#pragma once

#include <playground_plug/shared/FFPluginConfig.hpp>
#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base/dsp/fixed/FBFixedConfig.hpp>
#include <playground_base/dsp/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/fixed/FBFixedAudioBlock.hpp>
#include <playground_base/base/shared/FBStringify.hpp>

#include <array>

FBStaticTopo
FFMakeTopo();

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };

enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciAccGain, FFOsciAccPitch, FFOsciAccCount };
enum { FFOsciBlockOn, FFOsciBlockType, FFOsciBlockCount };

enum { FFShaperAccGain, FFShaperAccCount };
enum { FFShaperBlockOn, FFShaperBlockClip, FFShaperBlockCount };

template <class T>
struct alignas(alignof(T)) FFOsciAccMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccMemory);
  std::array<std::array<T, 1>, FF_OSCI_COUNT> pitch;
  std::array<std::array<T, FF_OSCI_GAIN_COUNT>, FF_OSCI_COUNT> gain;
};

struct FFOsciBlockMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockMemory);
  std::array<std::array<float, 1>, FF_OSCI_COUNT> on;
  std::array<std::array<float, 1>, FF_OSCI_COUNT> type;
};

template <class T>
struct alignas(alignof(T)) FFShaperAccMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccMemory);
  std::array<std::array<T, 1>, FF_SHAPER_COUNT> gain;
};

struct FFShaperBlockMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockMemory);
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> on;
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> clip;
};

template <class T>
struct alignas(alignof(T)) FFAccMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFAccMemory);
  FFOsciAccMemory<T> osci;
  FFShaperAccMemory<T> shaper;
};

struct FFBlockMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFBlockMemory);
  FFOsciBlockMemory osci;
  FFShaperBlockMemory shaper;
};

struct FFScalarMemory:
public FBScalarAddrsBase
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarMemory);
  FFBlockMemory block;
  FFAccMemory<float> acc;
};

struct alignas(alignof(FBFixedCVBlock)) FFProcMemory:
public FFScalarMemory,
public FBProcAddrsBase
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcMemory);
  FFAccMemory<int> pos;
  FFAccMemory<FBFixedCVBlock> cv;
};