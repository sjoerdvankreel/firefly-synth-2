#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

#include <array>

struct FBVoiceManagerInfo
{
  bool active = {};
  FBVoiceInfo info = {};
};

class FBVoiceManager
{
  std::uint64_t _counter = {};
  std::array<std::uint64_t, FB_MAX_VOICES> _num = {};
  std::array<FBVoiceManagerInfo, FB_MAX_VOICES> _voices = {};

public:
  void Return(std::uint64_t leased);
  std::uint64_t Lease(FBNoteEvent const& event);

  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceManager);
  std::array<FBVoiceManagerInfo, FB_MAX_VOICES> const& 
  Voices() { return _voices; }
};