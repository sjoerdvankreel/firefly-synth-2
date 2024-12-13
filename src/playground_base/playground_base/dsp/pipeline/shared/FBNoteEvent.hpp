#pragma once

#include <playground_base/dsp/pipeline/shared/FBNote.hpp>

struct FBNoteEvent
{
  int pos = -1;
  bool on = false;
  FBNote note = {};
  float velo = 0.0f;
};