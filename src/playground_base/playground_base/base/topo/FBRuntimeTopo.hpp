#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

#include <map>
#include <vector>

struct FBProcStatePtrs;
struct FBScalarStatePtrs;

struct FBRuntimeTopo final
{
  FBStaticTopo const static_;
  std::vector<FBRuntimeModule> const modules;
  std::vector<FBRuntimeParam> const params;
  std::map<int, int> const tagToParam;

  FBRuntimeTopo(FBStaticTopo const& static_);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);

  FBProcStatePtrs MakeProcStatePtrs(void* state) const;
  FBScalarStatePtrs MakeScalarStatePtrs(void* state) const;

  std::string SaveState(FBScalarStatePtrs const& from) const;
  bool LoadState(std::string const& from, FBScalarStatePtrs& to) const;
  bool LoadStateWithDryRun(std::string const& from, FBProcStatePtrs& to) const;
  bool LoadStateWithDryRun(std::string const& from, FBScalarStatePtrs& to) const;
};