#pragma once

#include <string>

struct FFDiskWriterOptions
{
  int bpm;
  int midiNote;
  int sampleRate;
  int durationMilliseconds;
  std::string outputRawPath;
  std::string inputPatchPath;
};