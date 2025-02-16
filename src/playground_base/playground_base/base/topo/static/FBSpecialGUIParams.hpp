#pragma once

struct FBStaticTopo;
struct FBStaticGUIParam;

struct FBSpecialGUIParam final
{
  int paramIndex = -1;
  int moduleIndex = -1;
  float* state = nullptr;
  FBStaticGUIParam const& ParamTopo(FBStaticTopo const& topo) const;
};

struct FBSpecialGUIParams final
{
  FBSpecialGUIParam userScale = {};
};