#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBSpecialParams.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBScalarStateContainer;

class FBProcStateContainer final
{
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBSmoothProcessor;

  void* _rawState;
  FBSpecialParams _special;
  void (*_freeRawState)(void*);
  float _smoothingDurationSecs = {};
  std::vector<FBProcParamState> _params = {};

  std::vector<FBProcParamState>& Params() { return _params; }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBProcStateContainer);
  FBProcStateContainer(FBRuntimeTopo const& topo);
  ~FBProcStateContainer() { _freeRawState(_rawState); }

  void CopyFrom(FBScalarStateContainer const& scalar);
  void SetSmoothingCoeffs(float sampleRate, float durationSecs);

  void* Raw() { return _rawState; }
  FBSpecialParams const& Special() const { return _special; }
  std::vector<FBProcParamState> const& Params() const { return _params; }
};