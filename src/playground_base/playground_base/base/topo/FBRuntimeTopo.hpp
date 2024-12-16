#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <vector>
#include <unordered_map>

class FBProcStatePtrs;
class FBScalarStateContainer;

struct FBRuntimeTopo final
{
  FBStaticTopo const static_;
  std::vector<FBRuntimeModule> const modules;
  std::vector<FBRuntimeParam> const params;
  std::unordered_map<int, int> const tagToParam;

  FBRuntimeTopo(FBStaticTopo const& static_);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);

  FBProcStatePtrs MakeProcStatePtrs(void* state) const; // TODO
  std::string SaveState(FBProcStatePtrs const& from) const;
  std::string SaveState(FBScalarStateContainer const& from) const;

  bool LoadState(std::string const& from, FBScalarStateContainer& to) const;
  bool LoadStateWithDryRun(std::string const& from, FBProcStatePtrs& to) const;
};