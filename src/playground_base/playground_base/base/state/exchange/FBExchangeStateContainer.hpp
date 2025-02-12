#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/exchange/FBParamExchangeState.hpp>
#include <playground_base/base/state/exchange/FBModuleExchangeState.hpp>
#include <playground_base/base/state/exchange/FBParamActiveExchangeState.hpp>

#include <array>
#include <vector>

struct FBRuntimeTopo;
struct FBRuntimeParam;

class FBExchangeStateContainer final
{
  friend class FBHostProcessor;

  void* _rawState;
  void (*_freeRawState)(void*);

  float* _bpm = {};
  float* _sampleRate = {};
  std::vector<FBParamExchangeState> _params = {};
  std::vector<FBModuleExchangeState> _modules = {};
  std::array<FBVoiceInfo, FBMaxVoices>* _voices = {};

  std::vector<FBParamExchangeState>& Params() { return _params; }
  std::vector<FBModuleExchangeState>& Modules() { return _modules; }
  std::array<FBVoiceInfo, FBMaxVoices>& Voices() { return *_voices; }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBExchangeStateContainer);
  FBExchangeStateContainer(FBRuntimeTopo const& topo);
  ~FBExchangeStateContainer() { _freeRawState(_rawState); }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  float Bpm() const { return *_bpm; }
  float SampleRate() const { return *_sampleRate; }
  template <class T> T* As() { return static_cast<T*>(Raw()); }
  template <class T> T const* As() const { return static_cast<T const*>(Raw()); }

  std::vector<FBParamExchangeState> const& Params() const { return _params; }
  std::vector<FBModuleExchangeState> const& Modules() const { return _modules; }
  std::array<FBVoiceInfo, FBMaxVoices> const& Voices() const { return *_voices; }
  FBParamActiveExchangeState GetParamActiveState(FBRuntimeParam const* param) const;
};