#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeModule.hpp>
#include <playground_base/base/topo/runtime/FBParamTopoIndices.hpp>

#include <map>
#include <vector>
#include <unordered_map>

template <class TParam>
struct FBRuntimeParamsTopo final
{
  std::vector<TParam> params;
  std::unordered_map<int, int> paramTagToIndex;
  std::map<FBParamTopoIndices, int> paramTopoToRuntime;

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBRuntimeParamsTopo);
  FBRuntimeParamsTopo(std::vector<FBRuntimeModule> const& modules);
  TParam const* ParamAtTopo(FBParamTopoIndices const& topoIndices) const;
};