#pragma once

struct FBStaticTopo;
class FBGlobalBlockParamState;

struct FBSpecialParam final
{
  int paramIndex = -1;
  int moduleIndex = -1;
  FBGlobalBlockParamState* state = nullptr;

  float NormalizedToPlainLinear(FBStaticTopo const& topo) const;
};

struct FBSpecialParams final
{
  FBSpecialParam smoothing = {};
};