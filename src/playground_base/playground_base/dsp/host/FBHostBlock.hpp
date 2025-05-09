#pragma once

#include <playground_base/dsp/shared/FBNote.hpp>

struct FBNoteEvent final
{
  int pos = -1;
  bool on = false;
  FBNote note = {};
  float velo = 0.0f;
};

struct FBBlockAutoEvent final
{
  int param = -1;
  float normalized = 0.0f;
};

struct FBAccAutoEvent final
{
  int pos = -1;
  int param = -1;
  float value = 0.0f; // [0, 1]
};

struct FBAccModEvent final
{
  int pos = -1;
  int param = -1;
  FBNote note = {};
  float value = 0.0f; // [-1, 1]
};

inline bool
FBAccAutoEventIsSameStream(
  FBAccAutoEvent const& l, FBAccAutoEvent const& r)
{
  return l.param == r.param;
}

inline bool
FBAccAutoEventOrderByParamThenPos(
  FBAccAutoEvent const& l, FBAccAutoEvent const& r)
{
  if (l.param != r.param)
    return l.param < r.param;
  return l.pos < r.pos;
}

inline bool
FBAccAutoEventOrderByPosThenParam(
  FBAccAutoEvent const& l, FBAccAutoEvent const& r)
{
  if (l.pos != r.pos)
    return l.pos < r.pos;
  return l.param < r.param;
}

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
