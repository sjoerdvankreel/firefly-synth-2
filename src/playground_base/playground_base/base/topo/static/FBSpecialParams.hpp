#pragma once

struct FBStaticTopo;
struct FBStaticParam;
class FBGlobalBlockParamState;

struct FBSpecialParam final
{
  int paramIndex = -1;
  int moduleIndex = -1;
  FBGlobalBlockParamState* state = nullptr;
  FBStaticParam const& ParamTopo(FBStaticTopo const& topo) const;
};

struct FBSpecialParams final
{
  FBSpecialParam hostSmoothTime = {};
};