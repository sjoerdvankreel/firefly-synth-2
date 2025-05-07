#pragma once

#include <unordered_map>

struct FBVoiceInfo;
struct FBRuntimeTopo;
struct FBPlugInputBlock;

enum class FBRenderType
{
  Audio,
  GraphPrimary,
  GraphExchange
};

struct FBModuleProcState final
{
  void* procRaw = {};
  int moduleSlot = {};
  bool anyExchangeActive = {};
  void* exchangeToGUIRaw = {};
  FBVoiceInfo const* voice = {};
  FBRuntimeTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  void const* exchangeFromGUIRaw = {};
  FBRenderType renderType = FBRenderType::Audio;
  std::unordered_map<int, float>* outputParamsNormalized = {};

  template <class T> T* ProcAs() { return static_cast<T*>(procRaw); }
  template <class T> T const* ProcAs() const { return static_cast<T const*>(procRaw); }
  template <class T> T* ExchangeToGUIAs() { return static_cast<T*>(exchangeToGUIRaw); }
  template <class T> T const* ExchangeFromGUIAs() const { return static_cast<T const*>(exchangeFromGUIRaw); }
};