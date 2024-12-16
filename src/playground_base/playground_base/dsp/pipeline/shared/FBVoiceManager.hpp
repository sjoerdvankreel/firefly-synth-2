#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>

#include <array>

class FBProcStateContainer;

struct FBVoiceInfo
{
  bool active = {};
  int initialOffset = {};
  FBNoteEvent event = {};
};

class FBVoiceManager
{
  std::uint64_t _counter = {};
  FBProcStateContainer* const _state;
  std::array<std::uint64_t, FBMaxVoices> _num = {};
  std::array<FBVoiceInfo, FBMaxVoices> _voices = {};

public:
  FBVoiceManager(FBProcStateContainer* state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVoiceManager);

  void Lease(FBNoteEvent const& event);
  void ReturnOldest(FBNoteEvent const& event);

  std::array<FBVoiceInfo, FBMaxVoices> const&
  Voices() const { return _voices; }
};