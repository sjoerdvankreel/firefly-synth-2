#pragma once

#include <playground_base/dsp/pipeline/shared/FBNote.hpp>

struct FBAccModEvent
{
  int pos = -1;
  int index = -1;
  FBNote note = {};
  float offset = 0.0f;
};