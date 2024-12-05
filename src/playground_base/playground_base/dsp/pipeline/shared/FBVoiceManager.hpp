#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>

#include <array>

struct FBProcStatePtrs;

struct FBVoiceInfo
{
  bool active = {};
  int initialOffset = {};
  FBNoteEvent event = {};
};

class FBVoiceManager
{
  std::uint64_t _counter = {};
  FBProcStatePtrs const* const _state;
  std::array<std::uint64_t, FB_MAX_VOICES> _num = {};
  std::array<FBVoiceInfo, FB_MAX_VOICES> _voices = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVoiceManager);
  FBVoiceManager(FBProcStatePtrs const* state);

  void Lease(FBNoteEvent const& event);
  void ReturnOldest(FBNoteEvent const& event);

  std::array<FBVoiceInfo, FB_MAX_VOICES> const&
  Voices() const { return _voices; }
};