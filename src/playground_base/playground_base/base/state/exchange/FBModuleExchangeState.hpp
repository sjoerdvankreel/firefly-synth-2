#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

struct FBModuleProcExchangeState;

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

  FB_NOCOPY_MOVE_DEFCTOR(FBModuleExchangeState);
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
  assert(IsGlobal());
  return _global;
}

inline FBModuleProcExchangeState const*
FBModuleExchangeState::Global() const
{
  assert(IsGlobal());
  return _global;
}

inline std::array<FBModuleProcExchangeState*, FBMaxVoices>&
FBModuleExchangeState::Voice()
{
  assert(!IsGlobal());
  return _voice;
}

inline std::array<FBModuleProcExchangeState*, FBMaxVoices> const&
FBModuleExchangeState::Voice() const
{
  assert(!IsGlobal());
  return _voice;
}