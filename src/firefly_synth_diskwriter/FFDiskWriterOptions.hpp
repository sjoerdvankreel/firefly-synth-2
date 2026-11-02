#pragma once

#include <string>

struct FFDiskWriterOptions
{
  int sampleRate;
  int baseMIDINote;
  int durationMilliseconds;
  std::string outputRawPath;
  std::string inputPatchPath;
};