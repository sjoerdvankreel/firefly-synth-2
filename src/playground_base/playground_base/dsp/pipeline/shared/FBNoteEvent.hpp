#pragma once

#include <playground_base/dsp/pipeline/shared/FBNote.hpp>

struct FBNoteEvent
{
  int pos;
  bool on;
  float velo;
  FBNote note;
};