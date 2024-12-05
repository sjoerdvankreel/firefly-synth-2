#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBStaticTopo;
struct FBPlugInputBlock;
struct FFProcParamState;

struct FFModuleProcState
{
  int moduleSlot = {};
  float sampleRate = {};
  FFProcParamState* state = {};
  FBStaticTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModuleProcState);
};