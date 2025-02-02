#pragma once

#include <unordered_map>

struct FBVoiceInfo;
struct FBRuntimeTopo;
struct FBPlugInputBlock;

struct FBModuleProcState final
{
  void* procRaw = {};
  int moduleSlot = {};
  float sampleRate = {};
  void* exchangeRaw = {};
  FBVoiceInfo const* voice = {};
  FBRuntimeTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  std::unordered_map<int, float>* outputParamsNormalized = {};

  template <class ProcStateT>
  ProcStateT* ProcState() const 
  { return static_cast<ProcStateT*>(procRaw); }
  template <class ExchangeStateT>
  ExchangeStateT* ExchangeState() const 
  { return static_cast<ExchangeStateT*>(exchangeRaw); }
};