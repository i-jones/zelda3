#include "DiffRecorder.hpp"

DiffRecorder::DiffRecorder(std::unique_ptr<PPUBase> ppu) : ParentClass(std::move(ppu))
{
    endRecording(std::nullopt);
}

void DiffRecorder::startRecording()
{
}

void DiffRecorder::discardRecording()
{
    endRecording(std::nullopt);
}

void DiffRecorder::writeRecording()
{
    endRecording(std::nullopt);
}
