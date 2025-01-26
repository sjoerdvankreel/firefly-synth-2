#pragma once

struct FBVoiceInfo;
struct FFProcState;
struct FBStaticTopo;
struct FBPlugInputBlock;

struct FFModuleProcState final
{
  int moduleSlot = {};
  float sampleRate = {};
  FFProcState* proc = {};
  FBVoiceInfo const* voice = {};
  FBStaticTopo const* topo = {};
  FBPlugInputBlock const* input = {};
};