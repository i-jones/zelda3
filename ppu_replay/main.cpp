#include <iostream>

#include "CLI/CLI.hpp"

// #include "PPUCommands_generated.h"

#include "ppu_lib/generated/PPUCommands_generated.h"
#include "ppu_lib/DebugPPU.hpp"
#include "ppu_lib/SNESPPU.hpp"

namespace
{
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
        std::cerr << "Command Type:  " << PPUCommand::EnumNameCommandType(cmd->command_type()) << "\n";
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
            validate(read->result(), result);
            break;
        }
        case PPUCommand::CommandType_Write:
        {
            auto write = cmd->command_as_Write();
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
            memcpy(ppu.vram, beginDrawing->vram()->data(), beginDrawing->vram()->size());
            ppu.beginDrawing(renderBuffer, pitch, beginDrawing->render_flags());
            break;
        }
        case PPUCommand::CommandType_GetCurrentRenderScale:
        {
            auto getCurrentRenderScale = cmd->command_as_GetCurrentRenderScale();
            auto result = ppu.getCurrentRenderScale(getCurrentRenderScale->render_flags());
            validate(getCurrentRenderScale->result(), result);
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
            validate(getMode->result(), result);
            break;
        }
        case PPUCommand::CommandType_GetExtraLeftRight:
        {
            auto getExtraLeftRight = cmd->command_as_GetExtraLeftRight();
            auto result = ppu.getExtraLeftRight();
            validate(getExtraLeftRight->result(), result);
            break;
        }
        case PPUCommand::CommandType_SetExtraLeftRight:
        {
            auto setExtraLeftRight = cmd->command_as_SetExtraLeftRight();
            ppu.setExtraLeftRight(setExtraLeftRight->extra_left_right());
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
        std::array<std::byte, 256 * 224 * 4> renderBuffer;

        auto *cmdVec = cmds->commands();
        std::cerr << "Number of commands: " << cmdVec->size() << "\n";
        for (auto cmd : *cmdVec)
        {
            std::cerr << "Command Type:  " << PPUCommand::EnumNameCommandType(cmd->command_type()) << "\n";
        }
        for (auto cmd : *cmdVec)
        {
            executeCmd(cmd, ppu, reinterpret_cast<uint8_t *>(renderBuffer.data()), 256 * 4);
        }
    }
}

int main(int argc, char *argv[])
{
    CLI::App app;

    std::string inputFile;

    app.add_option("-f", inputFile, "file to replay")->required();

    CLI11_PARSE(app, argc, argv);
    std::cout << "Replaying: " << inputFile << " \n";

    // Load the recording
    std::ifstream input(inputFile, std::ios::binary);

    // copies all data into buffer
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    input.close();

    flatbuffers::Verifier verifier(buffer.data(), buffer.size());
    if (!PPUCommand::VerifyCommandListBuffer(verifier))
    {
        std::cerr << "Invalid ppu file";
        std::exit(1);
    }

    auto commandList = PPUCommand::GetCommandList(buffer.data());

    DebugPPU<SNESPPU> ppu;

    // Execute
    runReplay(commandList, ppu, true);

    return 0;
}