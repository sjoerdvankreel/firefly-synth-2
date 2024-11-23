#pragma once

#include <playground_plug/plug/shared/FFPluginConfig.hpp>
#include <playground_plug/base/shared/FFBaseTopo.hpp>
#include <playground_plug/base/shared/FFSignalBlock.hpp>
#include <playground_plug/base/shared/FFSharedUtility.hpp>

#include <array>

FFStaticTopo
FFMakeTopo();

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };

enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciAccParamGain, FFOsciAccParamPitch, FFOsciAccParamCount };
enum { FFOsciBlockParamOn, FFOsciBlockParamType, FFOsciBlockParamCount };

enum { FFShaperAccParamGain, FFShaperAccParamCount };
enum { FFShaperBlockParamOn, FFShaperBlockParamClip, FFShaperBlockParamCount };

struct FFBlockParamMemory
{
  FF_NOCOPY_NOMOVE_DEFCTOR(FFBlockParamMemory);
  std::array<std::array<float, FFOsciBlockParamCount>, FF_OSCI_COUNT> osci;
  std::array<std::array<float, FFShaperBlockParamCount>, FF_SHAPER_COUNT> shaper;
};

template <class T>
struct FFAccParamMemory
{
  FF_NOCOPY_NOMOVE_DEFCTOR(FFAccParamMemory);
  std::array<std::array<T, FFOsciAccParamCount>, FF_OSCI_COUNT> osci;
  std::array<std::array<T, FFShaperAccParamCount>, FF_SHAPER_COUNT> shaper;
};

struct alignas(alignof(FFFixedMonoBlock)) FFProcessorParamMemory
{
  FFAccParamMemory<FFFixedMonoBlock> accParam;
  FFBlockParamMemory blockParam;
};

#if 0

#pragma once

#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginConfig.hpp>

#include <array>

typedef FBMonoBlock<FF_BLOCK_SIZE> FFMonoBlock;
typedef FBStereoBlock<FF_BLOCK_SIZE> FFStereoBlock;

struct FFPluginProcessors
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPluginProcessors);
  std::array<FFOsciProcessor, FF_OSCI_COUNT> osci;
  std::array<FFShaperProcessor, FF_SHAPER_COUNT> shaper;
};

struct FFScalarParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFScalarParamMemory);
  std::array<std::array<float, FFOsciPlugParamCount>, FF_OSCI_COUNT> osciPlug;
  std::array<std::array<float, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciAuto;
  std::array<std::array<float, FFShaperPlugParamCount>, FF_SHAPER_COUNT> shaperPlug;
  std::array<std::array<float, FFShaperAutoParamCount>, FF_SHAPER_COUNT> shaperAuto;
};

struct FFDenseParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFDenseParamMemory);
  std::array<std::array<int, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciEvent;
  std::array<std::array<int, FFShaperAutoParamCount>, FF_SHAPER_COUNT> shaperEvent;
  std::array<std::array<FFMonoBlock, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciDense;
  std::array<std::array<FFMonoBlock, FFShaperAutoParamCount>, FF_OSCI_COUNT> shaperDense;
};

struct FFProcessorMemory
{
  static constexpr int BlockSize = FF_BLOCK_SIZE;
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFProcessorMemory);

  FFStereoBlock masterOut;
  FFDenseParamMemory denseParam;
  FFScalarParamMemory scalarParam;

  std::array<FFStereoBlock, FF_OSCI_COUNT> osciOut;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperIn;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperOut;
};

typedef FBStaticTopo<FFProcessorMemory> FFStaticTopo;
typedef FBStaticParam<FFProcessorMemory> FFStaticParam;
typedef FBStaticModule<FFProcessorMemory> FFStaticModule;

typedef FBRuntimeTopo<FFProcessorMemory> FFRuntimeTopo;
typedef FBRuntimeParam<FFProcessorMemory> FFRuntimeParam;
typedef FBRuntimeModule<FFProcessorMemory> FFRuntimeModule;

FFStaticTopo
FFMakeTopo();


#endif