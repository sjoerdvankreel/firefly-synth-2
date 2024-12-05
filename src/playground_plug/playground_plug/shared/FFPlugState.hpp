#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFGLFOProcessor.hpp>
#include <playground_plug/dsp/FFShaperProcessor.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

#include <array>

template <class T>
struct alignas(alignof(T)) FFGLFOAccParamState final
{
  std::array<T, 1> rate = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccParamState);
};

template <class T>
struct FFGLFOParamState final
{
  FFGLFOAccParamState<T> acc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOParamState);
};

struct FFOsciBlockParamState final
{
  std::array<float, 1> on = {};
  std::array<float, 1> type = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class T>
struct alignas(alignof(T)) FFOsciAccParamState final
{
  std::array<T, 1> pitch = {};
  std::array<T, FF_OSCI_GAIN_COUNT> gain = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class T>
struct FFOsciParamState final
{
  FFOsciAccParamState<T> acc = {};
  FFOsciBlockParamState block = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};

struct FFShaperBlockParamState final
{
  std::array<float, 1> on = {};
  std::array<float, 1> clip = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockParamState);
};

template <class T>
struct alignas(alignof(T)) FFShaperAccParamState final
{
  std::array<T, 1> gain = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccParamState);
};

template <class T>
struct alignas(alignof(T)) FFShaperParamState final
{
  FFShaperAccParamState<T> acc = {};
  FFShaperBlockParamState block = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperParamState);
};

template <class T>
struct alignas(alignof(T)) FFPlugVoiceParamState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPlugVoiceParamState);
  std::array<FFOsciParamState<T>, FF_OSCI_COUNT> osci = {};
  std::array<FFShaperParamState<T>, FF_SHAPER_COUNT> shaper = {};
};

template <class T>
struct alignas(alignof(T)) FFPlugParamState:
public FFPlugVoiceParamState<T>
{
  std::array<FFGLFOParamState<T>, FF_GLFO_COUNT> glfo = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPlugParamState);
};

struct alignas(alignof(FBProcParamState)) FFProcParamState final:
public FFPlugParamState<FBProcParamState>
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcParamState);
  std::array<FFPlugVoiceParamState<FBProcParamState>, FB_MAX_VOICES> voices = {};
};

struct FFScalarState final
{
  FFPlugParamState<float> param;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

struct FFProcDSPState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcDSPState);
  std::array<FFGLFOProcessor, FF_GLFO_COUNT> glfo;
};

struct FFProcVoiceDSPState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcVoiceDSPState);
  std::array<FFOsciProcessor, FF_OSCI_COUNT> osci;
  std::array<FFShaperProcessor, FF_SHAPER_COUNT> shaper;
};

struct FFProcVoicesDSPState final:
public FFProcDSPState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcVoicesDSPState);
  std::array<FFProcVoiceDSPState, FB_MAX_VOICES> voices;
};

struct alignas(alignof(FBProcParamState)) FFProcState
{
  FFProcParamState param;
  FFProcVoicesDSPState dsp;
};