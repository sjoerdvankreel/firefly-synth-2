#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <map>
#include <vector>

struct FBRuntimeTopo
{
  FBStaticTopo const static_;
  std::vector<FBRuntimeModule> const modules;
  std::vector<FBRuntimeParam> const acc;
  std::vector<FBRuntimeParam> const block;
  std::map<int, int> const tagToAcc;
  std::map<int, int> const tagToBlock;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);
  FBRuntimeTopo(FBStaticTopo const& static_);

  void InitProcAddrs(FBProcStateAddrs& state) const;
  void InitScalarAddrs(FBScalarStateAddrs& state) const;

  std::string SaveState(FBScalarStateAddrs const& state) const;
  void LoadState(std::string const& stored, FBScalarStateAddrs const& state) const;
};