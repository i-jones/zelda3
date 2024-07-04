#pragma once

#include "PPURecorder.hpp"

class DiffRecorder : PPURecorder
{
public:
    DiffRecorder(std::unique_ptr<PPUBase> ppu);
    void startRecording();
    void discardRecording();
    void writeRecording();

private:
    using ParentClass = PPURecorder;
};