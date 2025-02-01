#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/state/FBExchangeParamState.hpp>

#include <array>
#include <vector>

struct FBRuntimeTopo;

class FBExchangeStateContainer final
{
  friend class FBHostProcessor;

  void* _rawState;
  void (*_freeRawState)(void*);
  std::vector<FBExchangeParamState> _params = {};
  std::array<bool, FBMaxVoices>* _voiceActive = {};

  std::vector<FBExchangeParamState>& Params() { return _params; }
  std::array<bool, FBMaxVoices>& VoiceActive() { return *_voiceActive; }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBExchangeStateContainer);
  FBExchangeStateContainer(FBRuntimeTopo const& topo);
  ~FBExchangeStateContainer() { _freeRawState(_rawState); }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  std::vector<FBExchangeParamState> const& Params() const { return _params; }
  std::array<bool, FBMaxVoices> const& VoiceActive() const { return *_voiceActive; }
};