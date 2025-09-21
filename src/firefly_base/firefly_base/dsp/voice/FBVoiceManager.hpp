#pragma once

#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <vector>
#include <cstdint>

class FBProcStateContainer;

inline int constexpr FBMaxVoices = 64;

enum class FBVoiceState { Free, Active, Returned };

struct FBVoiceInfo final
{
  int slot = {};
  int offsetInBlock = {};
  FBNoteEvent event = {};
  FBVoiceState state = {};

  // support for global unison, handled in the plug
  int slotInGroup = -1;
  std::int64_t groupId = -1;
};

class FBVoiceManager final
{
  friend class FBHostProcessor;
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
  int Lease(FBNoteEvent const& event, std::int64_t groupId, int slotInGroup);

  int VoiceCount() const { return _voiceCount; }
  std::vector<FBNote> const& ReturnedVoices() { return _returnedVoices; }
  std::array<FBVoiceInfo, FBMaxVoices> const& Voices() const { return _voices; }

  bool IsFree(int slot) const { return _voices[slot].state == FBVoiceState::Free; }
  bool IsActive(int slot) const { return _voices[slot].state == FBVoiceState::Active; }
  bool IsReturned(int slot) const { return _voices[slot].state == FBVoiceState::Returned; }
};