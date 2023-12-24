#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

#include <gtest/gtest.h>

#include "CLI/CLI.hpp"

#include "ppu_lib/generated/PPUCommands_generated.h"
#include "ppu_lib/DebugPPU.hpp"
#include "ppu_lib/SNESPPU.hpp"
#include "ppu_lib/reference/ppu.hpp"
#include "ppu_lib/utils.hpp"

namespace
{

    std::string inputFile;
    std::string ppuType;

    template <typename T>
    void validate(T expected, T actual)
    {
        if (actual != expected)
        {
            std::exit(1);
        }
    }

    void executeCmd(const PPUCommand::Command *cmd, PPUBase &ppu, uint8_t *renderBuffer, size_t pitch)
    {
        // std::cerr << "Command Type:  " << PPUCommand::EnumNameCommandType(cmd->command_type()) << "\n";
        switch (cmd->command_type())
        {
        case PPUCommand::CommandType_Reset:
            ppu.reset();
            break;
        case PPUCommand::CommandType_RunLine:
        {
            auto runLine = cmd->command_as_RunLine();
            ppu.runLine(runLine->line());
            break;
        }
        case PPUCommand::CommandType_Read:
        {
            auto read = cmd->command_as_Read();
            auto result = ppu.read(read->addr());
            ASSERT_EQ(read->result(), result) << "Read " << read->addr();
            break;
        }
        case PPUCommand::CommandType_Write:
        {
            auto write = cmd->command_as_Write();
            // std::cerr << "  Adr " << std::hex << (write->adr() + 0x2100) << "\n";
            ppu.write(write->adr(), write->value());
            break;
        }
        case PPUCommand::CommandType_WriteCGRam:
        {
            auto writeCGRam = cmd->command_as_WriteCGRam();
            ppu.writeCGRam(writeCGRam->data()->data(), writeCGRam->data()->size());
            break;
        }
        case PPUCommand::CommandType_WriteOAM:
        {
            auto writeOAM = cmd->command_as_WriteOAM();
            ppu.writeOam(writeOAM->data()->data(), writeOAM->data()->size());
            break;
        }
        case PPUCommand::CommandType_BeginDrawing:
        {
            auto beginDrawing = cmd->command_as_BeginDrawing();
            // TODO validate against buffer params?
            if (beginDrawing->vram())
            {
                memcpy(ppu.vram, beginDrawing->vram()->data(), beginDrawing->vram()->size());
            }
            ppu.beginDrawing(renderBuffer, pitch, beginDrawing->render_flags());
            break;
        }
        case PPUCommand::CommandType_GetCurrentRenderScale:
        {
            auto getCurrentRenderScale = cmd->command_as_GetCurrentRenderScale();
            auto result = ppu.getCurrentRenderScale(getCurrentRenderScale->render_flags());
            ASSERT_EQ(getCurrentRenderScale->result(), result) << "GetCurrentRenderScale";
            break;
        }
        case PPUCommand::CommandType_SetMode7PerspectiveCorrection:
        {
            auto setMode7PerspectiveCorrection = cmd->command_as_SetMode7PerspectiveCorrection();
            ppu.setMode7PerspectiveCorrection(setMode7PerspectiveCorrection->low(), setMode7PerspectiveCorrection->high());
            break;
        }
        case PPUCommand::CommandType_SetExtraSideSpace:
        {
            auto setExtraSideSpace = cmd->command_as_SetExtraSideSpace();
            ppu.setExtraSideSpace(setExtraSideSpace->left(), setExtraSideSpace->right(), setExtraSideSpace->bottom());
            break;
        }
        case PPUCommand::CommandType_GetMode:
        {
            auto getMode = cmd->command_as_GetMode();
            auto result = ppu.getMode();
            ASSERT_EQ(getMode->result(), result) << "GetMode";
            break;
        }
        case PPUCommand::CommandType_GetExtraLeftRight:
        {
            auto getExtraLeftRight = cmd->command_as_GetExtraLeftRight();
            auto result = ppu.getExtraLeftRight();
            ASSERT_EQ(getExtraLeftRight->result(), result) << "GetExtraLeftRight";
            break;
        }
        case PPUCommand::CommandType_SetExtraLeftRight:
        {
            auto setExtraLeftRight = cmd->command_as_SetExtraLeftRight();
            ppu.setExtraLeftRight(setExtraLeftRight->extra_left_right());
            break;
        }
        case PPUCommand::CommandType_ValidateImage:
        {
            auto validateImage = cmd->command_as_ValidateImage();

            std::span<const uint8_t> reference(validateImage->image()->data(), validateImage->image()->size());
            std::span<const uint8_t> current(renderBuffer, 256 * 224 * 4);
            bool imagesEqual = Utils::areImagesEqual(reference, current);
            if (!imagesEqual)
            {
                // Write the image out
                std::cerr << "Current path is " << fs::current_path() << '\n';
                std::string inputStem = fs::path(inputFile).stem();
                {

                    std::ofstream ostrm("reference-" + inputStem + "-" + ppuType + ".ppm");
                    PPMwriter::writePPMImage(ostrm, reference.data(), 256 * 4, std::nullopt);
                    ostrm.close();
                }
                {

                    std::ofstream ostrm("test-" + inputStem + "-" + ppuType + ".ppm");
                    PPMwriter::writePPMImage(ostrm, current.data(), 256 * 4, std::nullopt);
                    ostrm.close();
                }
            }
            ASSERT_TRUE(imagesEqual) << "Image differ";
            break;
        }
        default:
            std::cerr << "Unknown command: " << PPUCommand::EnumNameCommandType(cmd->command_type());
            throw std::runtime_error("Unhandled command");
            break;
        }
    }

    void runReplay(const PPUCommand::CommandList *cmds, PPUBase &ppu, bool validate)
    {
        // Setup render buffer
        std::array<std::byte, 256 * 225 * 4> renderBuffer{};

        auto *cmdVec = cmds->commands();
        std::cerr << "Number of commands: " << cmdVec->size() << "\n";

        for (auto cmd : *cmdVec)
        {
            ASSERT_NO_FATAL_FAILURE(executeCmd(cmd, ppu, reinterpret_cast<uint8_t *>(renderBuffer.data()), 256 * 4));
        }
    }
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    CLI::App app;

    app.add_option("-f", inputFile, "file to replay")->required();
    app.add_option("-p", ppuType, "ppu impl")->default_val("snes")->required();

    CLI11_PARSE(app, argc, argv);

    return RUN_ALL_TESTS();
}

TEST(PPUReplay, Replay)
{
    std::unique_ptr<PPUBase> ppu;
    if (ppuType == "snes")
    {
        ppu = std::make_unique<DebugPPU<SNESPPU>>();
    }
    else if (ppuType == "reference")
    {
        ppu = std::make_unique<DebugPPU<ReferencePPU>>();
    }
    ASSERT_NE(ppu, nullptr) << "Invalid ppu";
    std::cout << "Replaying: " << inputFile << " with ppu " << ppuType << "\n";

    // Load the recording data into buffer
    std::vector<unsigned char> buffer = Utils::loadFile(inputFile);

    flatbuffers::Verifier verifier(buffer.data(), buffer.size());
    ASSERT_TRUE(PPUCommand::VerifyCommandListBuffer(verifier)) << "Invalid ppu recording";

    auto commandList = PPUCommand::GetCommandList(buffer.data());

    // Execute
    ASSERT_NO_FATAL_FAILURE(runReplay(commandList, *ppu, true));
}