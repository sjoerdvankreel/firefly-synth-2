#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

#include <array>

struct FBModuleProcExchangeState
{
  bool active = {};
  int lengthSamples = {};
  int positionSamples = {};
  FB_COPY_MOVE_DEFCTOR(FBModuleProcExchangeState);

  bool ShouldGraph() const { return active && positionSamples < lengthSamples; }
  float PositionNormalized() const { return positionSamples / static_cast<float>(lengthSamples); }
};

class FBModuleExchangeState final
{
  friend class FBHostProcessor;
  friend class FBExchangeStateContainer;

  bool _isGlobal;
  union
  {
    FBModuleProcExchangeState* _global;
    std::array<FBModuleProcExchangeState*, FBMaxVoices> _voice;
  };

  FBModuleProcExchangeState* Global();
  std::array<FBModuleProcExchangeState*, FBMaxVoices>& Voice();

public:
  bool IsGlobal() const { return _isGlobal; }
  FBModuleProcExchangeState const* Global() const;
  std::array<FBModuleProcExchangeState*, FBMaxVoices> const& Voice() const;

  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleExchangeState);
  explicit FBModuleExchangeState(FBModuleProcExchangeState* global);
  explicit FBModuleExchangeState(std::array<FBModuleProcExchangeState*, FBMaxVoices> const& voice);
};

inline FBModuleExchangeState::
FBModuleExchangeState(FBModuleProcExchangeState* global) :
_isGlobal(true),
_global(global) {}

inline FBModuleExchangeState::
FBModuleExchangeState(std::array<FBModuleProcExchangeState*, FBMaxVoices> const& voice) :
_isGlobal(false),
_voice(voice) {}

inline FBModuleProcExchangeState*
FBModuleExchangeState::Global()
{
  FB_ASSERT(IsGlobal());
  return _global;
}

inline FBModuleProcExchangeState const*
FBModuleExchangeState::Global() const
{
  FB_ASSERT(IsGlobal());
  return _global;
}

inline std::array<FBModuleProcExchangeState*, FBMaxVoices>&
FBModuleExchangeState::Voice()
{
  FB_ASSERT(!IsGlobal());
  return _voice;
}

inline std::array<FBModuleProcExchangeState*, FBMaxVoices> const&
FBModuleExchangeState::Voice() const
{
  FB_ASSERT(!IsGlobal());
  return _voice;
}