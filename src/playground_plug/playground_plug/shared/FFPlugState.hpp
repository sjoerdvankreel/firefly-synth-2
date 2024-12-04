#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

#include <array>

template <class T>
struct alignas(alignof(T)) FFGLFOAccState final
{
  std::array<T, 1> rate = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccState);
};

template <class T>
struct FFGLFOState final
{
  FFGLFOAccState<T> acc;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOState);
};

struct FFOsciBlockState final
{
  std::array<float, 1> on = {};
  std::array<float, 1> type = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockState);
};

template <class T>
struct alignas(alignof(T)) FFOsciAccState final
{
  std::array<T, 1> pitch = {};
  std::array<T, FF_OSCI_GAIN_COUNT> gain = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccState);
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
  std::array<float, 1> on = {};
  std::array<float, 1> clip = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockState);
};

template <class T>
struct alignas(alignof(T)) FFShaperAccState final
{
  std::array<T, 1> gain = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccState);
};

template <class T>
struct alignas(alignof(T)) FFShaperState final
{
  FFShaperAccState<T> acc;
  FFShaperBlockState block;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperState);
};

template <class T>
struct alignas(alignof(T)) FFPlugVoiceState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPlugVoiceState);
  std::array<FFOsciState<T>, FF_OSCI_COUNT> osci;
  std::array<FFShaperState<T>, FF_SHAPER_COUNT> shaper;
};

template <class T>
struct alignas(alignof(T)) FFPlugState final:
public FFPlugVoiceState<T>
{
  std::array<FFGLFOState<T>, FF_GLFO_COUNT> glfo;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPlugState);
};

struct alignas(alignof(FBProcParamState)) FFPlugVoicesState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPlugVoicesState);
  std::array<FFPlugVoiceState<FBProcParamState>, FB_MAX_VOICES> voices;
};

typedef FFPlugState<float> FFScalarState;
typedef FFPlugState<FBProcParamState> FFProcState;
