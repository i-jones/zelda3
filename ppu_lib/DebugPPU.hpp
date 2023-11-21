#pragma once

#include <fstream>
#include <sstream>

#include "PPMWriter.hpp"

template <typename TPPU>
class DebugPPU : public TPPU
{
    using Parent = TPPU;

public:
    void beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
    {
        Parent::beginDrawing(buffer, pitch, render_flags);
        _drawBuffer = buffer;
        _drawBufferPitch = pitch;
        _frameCount++;
    }

    void runLine(int line)
    {
        Parent::runLine(line);
        if (_drawBuffer && line == 224)
        {
            // Write out the image;
            std::ofstream ostrm("lastRender.ppm");
            std::stringstream commentStream;
            commentStream << "frame number: " << _frameCount;
            PPMwriter::writePPMImage(ostrm, _drawBuffer, _drawBufferPitch, commentStream.str());
        }
        }

private:
    uint8_t *_drawBuffer = nullptr;
    size_t _drawBufferPitch = 0;
    int32_t _frameCount = 0;
};