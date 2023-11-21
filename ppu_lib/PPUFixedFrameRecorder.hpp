#pragma once

#include "PPURecorder.hpp"

class PPUFixedFrameRecorder : public PPURecorder
{
    using Parent = PPURecorder;

public:
    PPUFixedFrameRecorder(std::unique_ptr<PPUBase> ppu, int breakFrame);

    void beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags) override;
    void runLine(int line) override;

private:
    void writeCommandList();

    int _frameCount = 0;
    int _breakFrame = -1;
};