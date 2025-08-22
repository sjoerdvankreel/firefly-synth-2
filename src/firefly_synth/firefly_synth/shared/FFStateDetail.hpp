#pragma once

#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_base/base/state/proc/FBProcParamState.hpp>

template <bool Global>
inline auto
FFSelectDualState(
  auto selectGlobal, auto selectVoice)
{
  if constexpr (Global)
    return selectGlobal();
  else
    return selectVoice();
}

template <bool Global, class TExchangeState>
inline float&
FFSelectDualExchangeState(
  TExchangeState& state, int voice)
{
  if constexpr (Global)
    return state;
  else
    return state[voice];
}

template <bool Global, class TAccParamState>
inline auto const&
FFSelectDualProcAccParamNormalized(
  TAccParamState const& state, int voice)
{
  if constexpr (Global)
    return state.Global();
  else
    return state.Voice()[voice];
}

template <bool Global, class TBlockParamState>
inline float
FFSelectDualProcBlockParamNormalized(
  TBlockParamState const& state, int voice)
{
  if constexpr (Global)
    return state.Value();
  else
    return state.Voice()[voice];
}

template <bool Global, class TBlockParamState>
inline float
FFSelectDualProcBlockParamNormalizedGlobal(
  TBlockParamState const& state)
{
  if constexpr (Global)
    return state.Value();
  else
    return state.GlobalValue();
}