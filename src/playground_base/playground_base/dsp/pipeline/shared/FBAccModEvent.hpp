#pragma once

#include <playground_base/dsp/pipeline/shared/FBNote.hpp>

struct FBAccModEvent
{
  int pos = -1;
  int param = -1;
  FBNote note = {};
  float value = 0.0f; // [-1, 1]
};