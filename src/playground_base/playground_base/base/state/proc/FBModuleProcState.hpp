#pragma once

#include <playground_base/base/state/proc/FBRenderType.hpp>
#include <unordered_map>

struct FBVoiceInfo;
struct FBRuntimeTopo;
struct FBPlugInputBlock;

struct FBModuleProcState final
{
  void* procRaw = {};
  int moduleSlot = {};
  bool anyExchangeActive = {};
  void* exchangeToGUIRaw = {};
  void* exchangeFromGUIRaw = {};
  FBVoiceInfo const* voice = {};
  FBRuntimeTopo const* topo = {};
  FBPlugInputBlock const* input = {};
  FBRenderType renderType = FBRenderType::Audio;
  std::unordered_map<int, float>* outputParamsNormalized = {};

  template <class T> T* ProcAs() { return static_cast<T*>(procRaw); }
  template <class T> T const* ProcAs() const { return static_cast<T const*>(procRaw); }
  template <class T> T* ExchangeToGUIAs() { return static_cast<T*>(exchangeToGUIRaw); }
  template <class T> T* ExchangeFromGUIAs() { return static_cast<T*>(exchangeFromGUIRaw); }
  template <class T> T const* ExchangeToGUIAs() const { return static_cast<T const*>(exchangeToGUIRaw); }
  template <class T> T const* ExchangeFromGUIAs() const { return static_cast<T const*>(exchangeFromGUIRaw); }
};