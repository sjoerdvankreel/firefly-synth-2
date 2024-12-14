#pragma once

struct FBAccAutoEvent
{
  int pos = -1;
  int param = -1;
  float value = 0.0f; // [0, 1]
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