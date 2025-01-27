#pragma once

#include <unordered_map>

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
  std::unordered_map<int, float>* outputParamsNormalized = {};
};