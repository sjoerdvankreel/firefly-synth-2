#pragma once

#include <playground_plug/shared/FFPlugState.hpp>

template <class State>
inline auto
FFTopoDetailSelectAddr(
  auto selectModule, auto selectParam)
{
  return [selectModule, selectParam](int moduleSlot, int paramSlot, void* state) {
    auto moduleState = selectModule(static_cast<State*>(state)->param);
    return &(*selectParam((*moduleState)[moduleSlot]))[paramSlot]; };
}

inline auto
FFTopoDetailSelectScalarAddr(
  auto selectModule, auto selectParam)
{
  return FFTopoDetailSelectAddr<FFScalarState>(selectModule, selectParam);
}

inline auto
FFTopoDetailSelectProcAddr(
  auto selectModule, auto selectParam)
{
  return FFTopoDetailSelectAddr<FFProcState>(selectModule, selectParam);
}