#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBSpecialParams.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBScalarStatePtrs;

class FBProcStatePtrs final
{
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBSmoothProcessor;

  FBSpecialParams _special = {};
  std::vector<FBProcParamState> _params;
  std::vector<FBProcParamState>& Params() { return _params; }

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBProcStatePtrs);

  void InitDefaults(FBRuntimeTopo const& topo);
  void CopyFrom(FBScalarStatePtrs const& scalar);
  void SetSmoothingCoeffs(float sampleRate, float durationSecs);

  FBSpecialParams const& Special() const { return _special; }
  std::vector<FBProcParamState> const& Params() const { return _params; }
  FBProcStatePtrs(FBSpecialParams const& special, std::vector<FBProcParamState>&& params) : 
  _special(special), _params(std::move(params)) {}
};