#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeModule.hpp>

#include <map>
#include <vector>
#include <unordered_map>

template <class TParam>
struct FBRuntimeParamsTopo final
{
  // vector is always ok
  std::vector<TParam> params; 
  // this is for UI and host binding only, not perf-sensitive at time of writing
  std::unordered_map<int, int> paramTagToIndex;
  // this was slowing down global unison, so nested vector it is
  // std::map<FBParamTopoIndices, int> paramTopoToRuntime;
  // modindex-modslot-paramindex-paramslot
  std::vector<std::vector<std::vector<std::vector<int>>>> paramTopoToRuntime;

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBRuntimeParamsTopo);
  FBRuntimeParamsTopo(std::vector<FBRuntimeModule> const& modules);
  TParam const* ParamAtTopo(FBParamTopoIndices const& topoIndices) const;
};