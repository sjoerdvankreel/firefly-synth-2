#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBNote.hpp>

#include <array>
#include <vector>
#include <cstdint>

class FBBufferAudioBlock;

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

struct FBNoteEvent final
{
  int pos = -1;
  bool on = false;
  FBNote note = {};
  float velo = 0.0f;
};

struct FBMIDIEvent final
{
  static int constexpr CCMessageId = 0;
  static int constexpr CCMessageCount = 128;
  static int constexpr CPMessageId = CCMessageCount;
  static int constexpr PBMessageId = 129;
  static int constexpr MessageCount = 130;

  int pos = -1;
  int message = -1;
  int controlChange = 0;
  float value = 0.0f; // [0, 1]

  int EventId() const { return message + controlChange; }
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
  int sampleCount = {};
  FBHostAudioBlock audio = {};
  std::uint64_t projectTimeSamples = {};
  std::vector<FBNoteEvent> noteEvents = {};
  std::vector<FBBlockAutoEvent> blockAuto = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBMIDIEvent> midiByMessageThenCCThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  std::vector<FBAccModEvent> voiceStartModByParamThenNoteThenSample = {};

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

inline bool
FBMIDIEventIsSameStream(
  FBMIDIEvent const& l, FBMIDIEvent const& r)
{
  return l.message == r.message && l.controlChange == r.controlChange;
}

inline bool
FBMIDIEventOrderByMessageThenCCThenPos(
  FBMIDIEvent const& l, FBMIDIEvent const& r)
{
  if (l.message != r.message)
    return l.message < r.message;
  if (l.controlChange != r.controlChange)
    return l.controlChange < r.controlChange;
  return l.pos < r.pos;
}

inline bool
FBMIDIEventOrderByPosThenMessageThenCC(
  FBMIDIEvent const& l, FBMIDIEvent const& r)
{
  if (l.pos != r.pos)
    return l.pos < r.pos;
  if (l.message != r.message)
    return l.message < r.message;
  return l.controlChange < r.controlChange;
}