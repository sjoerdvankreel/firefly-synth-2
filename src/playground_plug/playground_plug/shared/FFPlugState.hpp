#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

#include <array>

struct FFOsciBlockState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockState);
  std::array<std::array<float, 1>, FF_OSCI_COUNT> on = {};
  std::array<std::array<float, 1>, FF_OSCI_COUNT> type = {};
};

template <class T>
struct alignas(alignof(T)) FFOsciAccState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccState);
  std::array<std::array<T, 1>, FF_OSCI_COUNT> pitch = {};
  std::array<std::array<T, FF_OSCI_GAIN_COUNT>, FF_OSCI_COUNT> gain = {};
};

template <class T>
struct FFOsciState final
{
  FFOsciAccState<T> acc;
  FFOsciBlockState block;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciState);
};

struct FFShaperBlockState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockState);
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> on = {};
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> clip = {};
};

template <class T>
struct alignas(alignof(T)) FFShaperAccState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccState);
  std::array<std::array<T, 1>, FF_SHAPER_COUNT> gain = {};
};

template <class T>
struct alignas(alignof(T)) FFShaperState final
{
  FFShaperAccState<T> acc;
  FFShaperBlockState block;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperState);
};

struct FFScalarState final
{
  FFOsciState<float> osci = {};
  FFShaperState<float> shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

struct alignas(alignof(FBFixedCVBlock)) FFProcState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
  FFOsciState<FBProcParamState> osci = {};
  FFShaperState<FBProcParamState> shaper = {};
};