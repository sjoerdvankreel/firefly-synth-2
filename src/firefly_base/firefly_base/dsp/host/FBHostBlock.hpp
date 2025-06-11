#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBNote.hpp>

#include <array>
#include <vector>
#include <cstdint>

class FBBufferAudioBlock;

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

class FBHostAudioBlock final
{
  int _count = 0;
  std::array<float*, 2> _store = {};

public:
  FB_COPY_MOVE_DEFCTOR(FBHostAudioBlock);
  FBHostAudioBlock(float** channels, int count);

  int Count() const { return _count; }
  void CopyFrom(FBBufferAudioBlock const& rhs, int count);
  float const* operator[](int ch) const { return _store[ch]; }
};

struct FBHostOutputBlock final
{
  FBHostAudioBlock audio = {};
  std::vector<FBNote> returnedVoices = {};
  std::vector<FBBlockAutoEvent> outputParams = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostOutputBlock);
};

struct FBHostInputBlock final
{
  float bpm = {};
  FBHostAudioBlock audio = {};
  std::vector<FBNoteEvent> noteEvents = {};
  std::vector<FBBlockAutoEvent> blockAuto = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};

  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostInputBlock);
  static inline float constexpr DefaultBPM = 120.0f;
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
