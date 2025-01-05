#pragma once

#include <playground_base/dsp/pipeline/shared/FBNote.hpp>

struct FBNoteEvent final
{
  int pos = -1;
  bool on = false;
  FBNote note = {};
  float velo = 0.0f;
};