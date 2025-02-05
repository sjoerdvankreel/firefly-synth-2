#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBExchangeParamState.hpp>
#include <playground_base/base/state/FBExchangeActiveState.hpp>

#include <array>
#include <vector>

struct FBRuntimeTopo;

class FBExchangeStateContainer final
{
  friend class FBHostProcessor;

  void* _rawState;
  void (*_freeRawState)(void*);
  std::vector<FBExchangeParamState> _params = {};
  std::vector<FBExchangeActiveState> _active = {};
  std::array<FBVoiceInfo, FBMaxVoices>* _voiceState = {};

  std::vector<FBExchangeParamState>& Params() { return _params; }
  std::vector<FBExchangeActiveState>& Active() { return _active; }
  std::array<FBVoiceInfo, FBMaxVoices>& VoiceState() { return *_voiceState; }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBExchangeStateContainer);
  FBExchangeStateContainer(FBRuntimeTopo const& topo);
  ~FBExchangeStateContainer() { _freeRawState(_rawState); }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  std::vector<FBExchangeParamState> const& Params() const { return _params; }
  std::vector<FBExchangeActiveState> const& Active() const { return _active; }
  std::array<FBVoiceInfo, FBMaxVoices> const& VoiceState() const { return *_voiceState; }
};