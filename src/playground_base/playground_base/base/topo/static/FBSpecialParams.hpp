#pragma once

struct FBStaticTopo;
struct FBStaticAudioParam;
class FBGlobalBlockParamState;

struct FBSpecialParam final
{
  int paramIndex = -1;
  int moduleIndex = -1;
  FBGlobalBlockParamState* state = nullptr;
  FBStaticAudioParam const& ParamTopo(FBStaticTopo const& topo) const;
};

struct FBSpecialParams final
{
  FBSpecialParam hostSmoothTime = {};
};