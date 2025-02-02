#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBSpecialParams.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBScalarStateContainer;
class FBExchangeStateContainer;

class FBProcStateContainer final
{
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBSmoothingProcessor;

  void* _rawState;
  FBSpecialParams _special;
  void (*_freeRawState)(void*);
  int _smoothingDurationSamples = {};
  std::vector<FBProcParamState> _params = {};
  std::vector<FBProcParamState>& Params() { return _params; }

  void InitProcessing(int index, int voice, float value);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBProcStateContainer);
  FBProcStateContainer(FBRuntimeTopo const& topo);
  ~FBProcStateContainer() { _freeRawState(_rawState); }

  void* Raw() { return _rawState; }
  FBSpecialParams const& Special() const { return _special; }
  std::vector<FBProcParamState> const& Params() const { return _params; }

  void SetSmoothingCoeffs(int sampleCount);
  void InitProcessing(int index, float value);
  void InitProcessing(FBScalarStateContainer const& scalar);
  void InitProcessing(FBExchangeStateContainer const& exchange);
};