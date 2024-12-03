#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

#include <array>
#include <memory>

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
struct FFShaperState final
{
  FFShaperAccState<T> acc;
  FFShaperBlockState block;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperState);
};

template <class T>
struct alignas(alignof(T)) FFAccState final
{
  FFOsciAccState<T> osci = {};
  FFShaperAccState<T> shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFAccState);
};

struct FFBlockState final
{
  FFOsciBlockState osci = {};
  FFShaperBlockState shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFBlockState);
};

struct FFScalarState final
{
  FFBlockState block = {};
  FFAccState<float> acc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

struct alignas(alignof(FBFixedCVBlock)) FFProcState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
  FFAccState<FBProcParamState> param = {};
};