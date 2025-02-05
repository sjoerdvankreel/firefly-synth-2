#pragma once

#include <playground_plug/shared/FFPlugState.hpp>

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