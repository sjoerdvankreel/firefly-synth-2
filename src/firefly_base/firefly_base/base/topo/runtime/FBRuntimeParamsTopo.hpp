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
  std::vector<TParam> params;
  std::unordered_map<int, int> paramTagToIndex;
  std::map<FBParamTopoIndices, int> paramTopoToRuntime;

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBRuntimeParamsTopo);
  FBRuntimeParamsTopo(std::vector<FBRuntimeModule> const& modules);
  TParam const* ParamAtTopo(FBParamTopoIndices const& topoIndices) const;
};