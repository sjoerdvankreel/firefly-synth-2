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

  // TODO just exchange, not in/out, null when graph
  void* exchangeOutputRaw = {};
  void const* exchangeInputRaw = {};
  FBVoiceInfo const* voice = {};
  FBRuntimeTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  std::unordered_map<int, float>* outputParamsNormalized = {};

  template <class ProcStateT>
  ProcStateT* ProcState() const 
  { return static_cast<ProcStateT*>(procRaw); }
  template <class ExchangeStateT>
  ExchangeStateT* ExchangeOutput() const 
  { return static_cast<ExchangeStateT*>(exchangeOutputRaw); }
  template <class ExchangeStateT>
  ExchangeStateT const* ExchangeInput() const 
  { return static_cast<ExchangeStateT const*>(exchangeInputRaw); }
};