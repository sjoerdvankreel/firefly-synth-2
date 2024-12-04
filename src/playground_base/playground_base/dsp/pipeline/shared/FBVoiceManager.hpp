#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>

#include <array>

struct FBVoiceInfo
{
  bool active = {};
  FBNoteEvent event = {};
};

class FBVoiceManager
{
  std::uint64_t _counter = {};
  std::array<std::uint64_t, FB_MAX_VOICES> _num = {};
  std::array<FBVoiceInfo, FB_MAX_VOICES> _voices = {};

public:
  void Return(int leased);
  int Lease(FBNoteEvent const& event);

  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceManager);
  std::array<FBVoiceInfo, FB_MAX_VOICES> const&
  Voices() { return _voices; }
};