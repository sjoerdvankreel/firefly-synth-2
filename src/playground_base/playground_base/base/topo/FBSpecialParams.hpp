#pragma once

struct FBStaticTopo;
class FBGlobalBlockParamState;

struct FBSpecialParam
{
  int paramIndex = -1;
  int moduleIndex = -1;
  FBGlobalBlockParamState* state = nullptr;

  float NormalizedToPlainLinear(FBStaticTopo const& topo) const;
};

struct FBSpecialParams
{
  FBSpecialParam smoothing = {};
};