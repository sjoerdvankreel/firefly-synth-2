#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

#include <vector>
#include <utility>

class FBScalarStatePtrs;

class FBProcStatePtrs final
{
  friend class FBVoiceManager;
  
  std::vector<FBProcParamState> _params;
  std::vector<FBProcParamState>& Params() { return _params; }

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBProcStatePtrs);
  void CopyFrom(FBScalarStatePtrs const& scalar);

  std::vector<FBProcParamState> const& Params() const { return _params; }
  FBProcStatePtrs(std::vector<FBProcParamState>&& params) : _params(std::move(params)) {}
};