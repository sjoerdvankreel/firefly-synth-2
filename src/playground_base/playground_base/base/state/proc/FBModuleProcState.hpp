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

  template <class T> T* ProcAs() const { return static_cast<T*>(procRaw); }
  template <class T> T* ExchangeAs() const { return static_cast<T*>(exchangeRaw); }
};