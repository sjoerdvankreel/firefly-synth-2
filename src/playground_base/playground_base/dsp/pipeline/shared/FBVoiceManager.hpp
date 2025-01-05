#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>

#include <array>
#include <vector>
#include <cstdint>

class FBProcStateContainer;

enum class FBVoiceState
{
  Free,
  Active,
  Returned
};

struct FBVoiceInfo final
{
  int initialOffset = {};
  FBNoteEvent event = {};
  FBVoiceState state = {};
};

class FBVoiceManager final
{
  std::uint64_t _counter = {};
  FBProcStateContainer* const _state;
  std::vector<FBNote> _returnedVoices = {};
  std::array<std::uint64_t, FBMaxVoices> _num = {};
  std::array<FBVoiceInfo, FBMaxVoices> _voices = {};

public:
  FBVoiceManager(FBProcStateContainer* state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVoiceManager);

  void Lease(FBNoteEvent const& event);
  void ReturnOldest(FBNoteEvent const& event);

  void ResetReturnedVoices();
  std::vector<FBNote> const& ReturnedVoices() { return _returnedVoices; }
  std::array<FBVoiceInfo, FBMaxVoices> const& Voices() const { return _voices; }

  bool IsFree(int slot) const { return _voices[slot].state == FBVoiceState::Free; }
  bool IsActive(int slot) const { return _voices[slot].state == FBVoiceState::Active; }
  bool IsReturned(int slot) const { return _voices[slot].state == FBVoiceState::Returned; }
};