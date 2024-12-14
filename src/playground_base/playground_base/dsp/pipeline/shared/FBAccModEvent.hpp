#pragma once

#include <playground_base/dsp/pipeline/shared/FBNote.hpp>

struct FBAccModEvent
{
  int pos = -1;
  int param = -1;
  FBNote note = {};
  float value = 0.0f; // [-1, 1]
};

inline bool
FBAccModEventIsSameStream(
  FBAccModEvent const& l, FBAccModEvent const& r)
{
  return l.param == r.param && l.note == r.note;
}

inline bool
FBAccModEventOrderByParamThenNoteThenPos(
  FBAccModEvent const& l, FBAccModEvent const& r)
{
  if (l.param != r.param)
    return l.param < r.param;
  if (l.note != r.note)
    return l.note < r.note;
  return l.pos < r.pos;
}

inline bool
FBAccModEventOrderByPosThenParamThenNote(
  FBAccModEvent const& l, FBAccModEvent const& r)
{
  if (l.pos != r.pos)
    return l.pos < r.pos;
  if (l.param != r.param)
    return l.param < r.param;
  return l.note < r.note;
}