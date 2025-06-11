#pragma once

#include <firefly_synth/shared/FFPlugState.hpp>

inline auto
FFSelectGlobalModuleExchangeAddr(
  auto selectModule)
{
  return [selectModule](int moduleSlot, void* state) {
    auto& globalState = static_cast<FFExchangeState*>(state)->global;
    return &(*selectModule(globalState))[moduleSlot]; };
}

inline auto
FFSelectVoiceModuleExchangeAddr(
  auto selectModule)
{
  return [selectModule](int voice, int moduleSlot, void* state) {
    auto& voiceState = static_cast<FFExchangeState*>(state)->voice[voice];
    return &(*selectModule(voiceState))[moduleSlot]; };
}

inline auto
FFSelectGUIParamAddr(
  auto selectModule, auto selectParam)
{
  return [selectModule, selectParam](int moduleSlot, int paramSlot, void* state) {
    auto moduleState = selectModule(*static_cast<FFGUIState*>(state));
    return &(*selectParam((*moduleState)[moduleSlot]))[paramSlot]; };
}

template <class State>
inline auto
FFSelectParamAddr(
  auto selectModule, auto selectParam)
{
  return [selectModule, selectParam](int moduleSlot, int paramSlot, void* state) {
    auto moduleState = selectModule(static_cast<State*>(state)->param);
    return &(*selectParam((*moduleState)[moduleSlot]))[paramSlot]; };
}

inline auto
FFSelectProcParamAddr(
  auto selectModule, auto selectParam)
{
  return FFSelectParamAddr<FFProcState>(selectModule, selectParam);
}

inline auto
FFSelectScalarParamAddr(
  auto selectModule, auto selectParam)
{
  return FFSelectParamAddr<FFScalarState>(selectModule, selectParam);
}

inline auto
FFSelectExchangeParamAddr(
  auto selectModule, auto selectParam)
{
  return FFSelectParamAddr<FFExchangeState>(selectModule, selectParam);
}

template <bool Global>
inline FBScalarParamAddrSelector
FFSelectDualScalarParamAddrImpl(auto selectGlobalModule, auto selectVoiceModule, auto selectParam)
{
  if constexpr (Global)
    return FFSelectScalarParamAddr(selectGlobalModule, selectParam);
  else
    return FFSelectScalarParamAddr(selectVoiceModule, selectParam);
}

inline FBScalarParamAddrSelector
FFSelectDualScalarParamAddr(bool global, auto selectGlobalModule, auto selectVoiceModule, auto selectParam)
{
  if(global)
    return FFSelectDualScalarParamAddrImpl<true>(selectGlobalModule, selectVoiceModule, selectParam);
  else
    return FFSelectDualScalarParamAddrImpl<false>(selectGlobalModule, selectVoiceModule, selectParam);
}