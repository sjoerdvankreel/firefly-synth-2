#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBParamExchangeState.hpp>
#include <playground_base/base/state/FBActiveExchangeState.hpp>
#include <playground_base/base/state/FBParamActiveExchangeState.hpp>

#include <array>
#include <vector>

struct FBRuntimeTopo;
struct FBRuntimeParam;

class FBExchangeStateContainer final
{
  friend class FBHostProcessor;

  void* _rawState;
  void (*_freeRawState)(void*);
  std::vector<FBParamExchangeState> _params = {};
  std::vector<FBActiveExchangeState> _active = {};
  std::array<FBVoiceInfo, FBMaxVoices>* _voiceState = {};

  std::vector<FBParamExchangeState>& Params() { return _params; }
  std::vector<FBActiveExchangeState>& Active() { return _active; }
  std::array<FBVoiceInfo, FBMaxVoices>& VoiceState() { return *_voiceState; }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBExchangeStateContainer);
  FBExchangeStateContainer(FBRuntimeTopo const& topo);
  ~FBExchangeStateContainer() { _freeRawState(_rawState); }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  std::vector<FBParamExchangeState> const& Params() const { return _params; }
  std::vector<FBActiveExchangeState> const& Active() const { return _active; }
  std::array<FBVoiceInfo, FBMaxVoices> const& VoiceState() const { return *_voiceState; }
  FBParamActiveExchangeState GetParamActiveState(FBRuntimeParam const* param) const;
};