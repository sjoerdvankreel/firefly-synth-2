#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFGLFOProcessor.hpp>
#include <playground_plug/dsp/FFShaperProcessor.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <array>

template <class T>
class alignas(alignof(T)) FFGLFOAccParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<T, 1> rate = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccParamState);
};

template <class T>
class alignas(alignof(T)) FFGLFOParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFGLFOAccParamState<T> acc = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOParamState);
};

class FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<float, 1> on = {};
  std::array<float, 1> type = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class T>
class alignas(alignof(T)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<T, 1> pitch = {};
  std::array<T, FF_OSCI_GAIN_COUNT> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class T>
class alignas(alignof(T)) FFOsciParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFOsciAccParamState<T> acc = {};
  FFOsciBlockParamState block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};

class FFShaperBlockParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<float, 1> on = {};
  std::array<float, 1> clip = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockParamState);
};

template <class T>
class alignas(alignof(T)) FFShaperAccParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<T, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccParamState);
};

template <class T>
class alignas(alignof(T)) FFShaperParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFShaperAccParamState<T> acc = {};
  FFShaperBlockParamState block = {};
public:
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
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPlugParamState);
  std::array<FFGLFOParamState<T>, FF_GLFO_COUNT> glfo = {};
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFProcParamState final:
public FFPlugParamState<FBProcParamState>
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcParamState);
  std::array<FFPlugVoiceParamState<FBProcParamState>, FB_MAX_VOICES> voices = {};
};

struct FFScalarState final
{
  FFPlugParamState<float> param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

class alignas(FB_FIXED_BLOCK_ALIGN) FFGLFODSPState final
{
  friend class FFGLFOProcessor;
  FBFixedCVBlock output = {};
public:
  FFGLFOProcessor processor = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFODSPState);
};

class alignas(FB_FIXED_BLOCK_ALIGN) FFOsciDSPState final
{
  friend class FFOsciProcessor;
  friend class FFPlugProcessor; // TODO
  FBFixedAudioBlock output = {};
public:
  FFOsciProcessor processor = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciDSPState);
};

class alignas(FB_FIXED_BLOCK_ALIGN) FFShaperDSPState final
{
  friend class FFShaperProcessor;
  FBFixedAudioBlock output = {};
public:
  FFShaperProcessor processor = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperDSPState);
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFProcVoiceDSPState final
{
  FBFixedAudioBlock output = {};
  std::array<FFOsciDSPState, FF_OSCI_COUNT> osci = {};
  std::array<FFShaperDSPState, FF_SHAPER_COUNT> shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcVoiceDSPState);
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFProcDSPState final
{
  std::array<FFGLFOProcessor, FF_GLFO_COUNT> glfo = {};
  std::array<FFProcVoiceDSPState, FB_MAX_VOICES> voices = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcDSPState);
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFProcState final
{
  FFProcDSPState dsp = {};
  FFProcParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
};