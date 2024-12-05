#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFProcState;
struct FBStaticTopo;
struct FBPlugInputBlock;

struct FFModuleProcState
{
  int moduleSlot = {};
  float sampleRate = {};
  FFProcState* state = {};
  FBStaticTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModuleProcState);
};