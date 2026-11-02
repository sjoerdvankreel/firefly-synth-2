#pragma once

#include <string>

struct FFDiskWriterOptions
{
  int bpm;
  int midiNote;
  int sampleRate;
  int holdDurationMilliseconds;
  int totalDurationMilliseconds;
  std::string outputWavPath;
  std::string inputPatchPath;
};