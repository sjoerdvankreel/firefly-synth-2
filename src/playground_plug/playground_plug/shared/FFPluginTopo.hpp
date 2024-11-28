#pragma once

#include <playground_plug/shared/FFPluginConfig.hpp>
#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/plug/FBPlugAudioBlock.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>

#include <array>

FBStaticTopo
FFMakeTopo();

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };

enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciAccParamGain, FFOsciAccParamPitch, FFOsciAccParamCount };
enum { FFOsciBlockParamOn, FFOsciBlockParamType, FFOsciBlockParamCount };

enum { FFShaperAccParamGain, FFShaperAccParamCount };
enum { FFShaperBlockParamOn, FFShaperBlockParamClip, FFShaperBlockParamCount };

template <class T>
struct alignas(alignof(T)) FFOsciAccParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamMemory);
  std::array<std::array<T, 1>, FF_OSCI_COUNT> pitch;
  std::array<std::array<T, FF_OSCI_GAIN_COUNT>, FF_OSCI_COUNT> gain;
};

struct FFOsciBlockParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamMemory);
  std::array<std::array<float, 1>, FF_OSCI_COUNT> on;
  std::array<std::array<float, 1>, FF_OSCI_COUNT> type;
};

template <class T>
struct alignas(alignof(T)) FFShaperAccParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccParamMemory);
  std::array<std::array<T, 1>, FF_SHAPER_COUNT> gain;
};

struct FFShaperBlockParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockParamMemory);
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> on;
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> clip;
};

template <class T>
struct alignas(alignof(T)) FFAccParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFAccParamMemory);
  FFOsciAccParamMemory<T> osci;
  FFShaperAccParamMemory<T> shaper;
};

struct FFBlockParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFBlockParamMemory);
  FFOsciBlockParamMemory osci;
  FFShaperBlockParamMemory shaper;
};

struct FFScalarParamMemory:
public FBScalarParamAddrsBase
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarParamMemory);
  FFBlockParamMemory block;
  FFAccParamMemory<float> acc;
};

struct alignas(alignof(FBPlugSignalBlock)) FFDenseParamMemory:
public FFScalarParamMemory,
public FBDenseParamAddrsBase
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFDenseParamMemory);
  FFAccParamMemory<int> pos;
  FFAccParamMemory<FBPlugSignalBlock> buffer;
};