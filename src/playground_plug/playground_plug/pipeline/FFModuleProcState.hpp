#pragma once

struct FBVoiceInfo;
struct FFProcState;
struct FBRuntimeTopo;
struct FBPlugInputBlock;

struct FFModuleProcState final
{
  int moduleSlot = {};
  float sampleRate = {};
  FFProcState* proc = {};
  FBVoiceInfo const* voice = {};
  FBRuntimeTopo const* topo = {};
  FBPlugInputBlock const* input = {};
};