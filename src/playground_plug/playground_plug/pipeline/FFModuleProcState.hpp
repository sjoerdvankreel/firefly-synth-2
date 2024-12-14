#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBVoiceInfo;
struct FFProcState;
struct FBStaticTopo;
struct FBPlugInputBlock;

struct FFModuleProcState
{
  int moduleSlot = {};
  float sampleRate = {};
  FFProcState* proc = {};
  FBVoiceInfo const* voice = {};
  FBStaticTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  FB_NOCOPY_MOVE_DEFCTOR(FFModuleProcState);
};