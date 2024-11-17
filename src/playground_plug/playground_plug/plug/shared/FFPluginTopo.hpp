#pragma once

#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginConfig.hpp>

#include <array>

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };
enum { FFShaperPlugParamOn, FFShaperPlugParamCount };
enum { FFShaperAutoParamGain, FFShaperAutoParamCount };
enum { FFOsciPlugParamOn, FFOsciPlugParamCount };
enum { FFOsciAutoParamGain, FFOsciAutoParamPitch, FFOsciAutoParamCount };

struct FFPluginProcessors
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPluginProcessors);

  std::array<FFOsciProcessor, FF_OSCI_COUNT> osci;
  std::array<FFShaperProcessor, FF_SHAPER_COUNT> shaper;
};

template <class T>
struct FFParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFParamMemory);

  std::array<std::array<T, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciAuto;
  std::array<std::array<T, FFShaperAutoParamCount>, FF_SHAPER_COUNT> shaperAuto;
  std::array<std::array<float, FFOsciPlugParamCount>, FF_OSCI_COUNT> osciPlug;
  std::array<std::array<float, FFShaperPlugParamCount>, FF_SHAPER_COUNT> shaperPlug;
};

typedef FBMonoBlock<FF_BLOCK_SIZE> FFMonoBlock;
typedef FBStereoBlock<FF_BLOCK_SIZE> FFStereoBlock;
typedef FFParamMemory<float> FFControllerParamMemory;
typedef FFParamMemory<FFMonoBlock> FFProcessorParamMemory;

struct FFPluginBlock
{
  static const int BlockSize = FF_BLOCK_SIZE;
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPluginBlock);

  float sampleRate;
  FFStereoBlock masterOut;
  FFProcessorParamMemory paramMemory;
  std::array<FFStereoBlock, FF_OSCI_COUNT> osciOut;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperIn;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperOut;
};

typedef FBStaticTopo<FFPluginBlock> FFStaticTopo;
typedef FBStaticParam<FFPluginBlock> FFStaticParam;
typedef FBStaticModule<FFPluginBlock> FFStaticModule;

typedef FBRuntimeTopo<FFPluginBlock> FFRuntimeTopo;
typedef FBRuntimeParam<FFPluginBlock> FFRuntimeParam;
typedef FBRuntimeModule<FFPluginBlock> FFRuntimeModule;

FFStaticTopo
FFMakeTopo();
