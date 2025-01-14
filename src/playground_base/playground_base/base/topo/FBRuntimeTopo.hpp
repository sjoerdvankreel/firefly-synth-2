#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <vector>
#include <unordered_map>

class FBProcStateContainer;
class FBScalarStateContainer;

struct FBRuntimeTopo final
{
  FBStaticTopo static_;
  std::vector<FBRuntimeModule> modules;
  std::vector<FBRuntimeParam> params;
  std::unordered_map<int, int> paramTagToIndex;

  FBRuntimeTopo(FBStaticTopo const& static_);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);

  std::string SaveProcState(FBProcStateContainer const& proc) const;
  std::string SaveEditState(FBScalarStateContainer const& edit) const;

  bool LoadEditState(std::string const& jsonText, FBScalarStateContainer& edit) const;
  bool LoadProcStateWithDryRun(std::string const& jsonText, FBProcStateContainer& proc) const;
  bool LoadEditStateWithDryRun(std::string const& jsonText, FBScalarStateContainer& edit) const;
};