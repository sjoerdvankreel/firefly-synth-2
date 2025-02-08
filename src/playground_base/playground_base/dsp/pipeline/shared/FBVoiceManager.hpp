#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

#include <array>
#include <vector>
#include <cstdint>

class FBProcStateContainer;

class FBVoiceManager final
{
  friend class FBGraphRenderState;

  int _voiceCount = 0;
  std::uint64_t _counter = {};
  FBProcStateContainer* const _procState;
  std::vector<FBNote> _returnedVoices = {};
  std::array<std::uint64_t, FBMaxVoices> _num = {};
  std::array<FBVoiceInfo, FBMaxVoices> _voices = {};

  void InitFromExchange(std::array<FBVoiceInfo, FBMaxVoices> const& voices);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVoiceManager);
  FBVoiceManager(FBProcStateContainer* procState);

  void Return(int slot);
  void ResetReturnedVoices();
  int Lease(FBNoteEvent const& event);

  int VoiceCount() const { return _voiceCount; }
  std::vector<FBNote> const& ReturnedVoices() { return _returnedVoices; }
  std::array<FBVoiceInfo, FBMaxVoices> const& Voices() const { return _voices; }

  bool IsFree(int slot) const { return _voices[slot].state == FBVoiceState::Free; }
  bool IsActive(int slot) const { return _voices[slot].state == FBVoiceState::Active; }
  bool IsReturned(int slot) const { return _voices[slot].state == FBVoiceState::Returned; }
};