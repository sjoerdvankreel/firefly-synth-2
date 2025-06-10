#pragma once

#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_base/base/state/proc/FBProcParamState.hpp>

template <bool Global>
inline auto const*
FFSelectDualProcParamState(
  auto selectGlobal, auto selectVoice)
{
  if constexpr (Global)
    return selectGlobal();
  else
    return selectVoice();
}

template <bool Global, class TBlockParamState>
inline auto const&
FFSelectDualProcBlockParamNormalized(
  TBlockParamState const& state, int voice)
{
  if constexpr (Global)
    return state.Value();
  else
    return state.Voice()[voice];
}