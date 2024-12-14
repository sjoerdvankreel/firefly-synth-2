#pragma once

#include <playground_plug/shared/FFPlugState.hpp>

template <class State>
static auto
FFTopoDetailSelectAddr(
  auto selectModule, auto selectParam)
{
  return [selectModule, selectParam](int moduleSlot, int paramSlot, void* state) {
    auto moduleState = selectModule(static_cast<State*>(state)->param);
    return &(*selectParam((*moduleState)[moduleSlot]))[paramSlot]; };
}

static auto
FFTopoDetailSelectScalarAddr(
  auto selectModule, auto selectParam)
{
  return FFTopoDetailSelectAddr<FFScalarState>(selectModule, selectParam);
}

static auto
FFTopoDetailSelectProcAddr(
  auto selectModule, auto selectParam)
{
  return FFTopoDetailSelectAddr<FFProcState>(selectModule, selectParam);
}