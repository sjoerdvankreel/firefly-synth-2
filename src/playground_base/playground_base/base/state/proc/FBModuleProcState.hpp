#pragma once

#include <unordered_map>

struct FBVoiceInfo;
struct FBRuntimeTopo;
struct FBPlugInputBlock;

struct FBModuleProcState final
{
  void* procRaw = {};
  int moduleSlot = {};
  void* exchangeRaw = {};
  FBVoiceInfo const* voice = {};
  FBRuntimeTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  std::unordered_map<int, float>* outputParamsNormalized = {};

  template <class T> T* ProcAs() { return static_cast<T*>(procRaw); }
  template <class T> T* ExchangeAs() { return static_cast<T*>(exchangeRaw); }
  template <class T> T const* ProcAs() const { return static_cast<T const*>(procRaw); }
  template <class T> T const* ExchangeAs() const { return static_cast<T const*>(exchangeRaw); }
};