#pragma once

#include <playground_plug/shared/FFPlugState.hpp>

inline auto
FFSelectGlobalExchangeActiveAddr(
  auto selectModule)
{
  return [selectModule](int moduleSlot, void* state) {
    auto& globalState = static_cast<FFExchangeState*>(state)->global;
    return &(*selectModule(globalState))[moduleSlot].active; };
}

inline auto
FFSelectVoiceExchangeActiveAddr(
  auto selectModule)
{
  return [selectModule](int voice, int moduleSlot, void* state) {
    auto& voiceState = static_cast<FFExchangeState*>(state)->voice[voice];
    return &(*selectModule(voiceState))[moduleSlot].active; };
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