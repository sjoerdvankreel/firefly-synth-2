#pragma once

#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginConfig.hpp>

#include <array>

typedef FBMonoBlock<FF_BLOCK_SIZE> FFMonoBlock;
typedef FBStereoBlock<FF_BLOCK_SIZE> FFStereoBlock;

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };

enum { FFShaperAutoParamGain, FFShaperAutoParamCount };
enum { FFShaperPlugParamOn, FFShaperPlugParamClip, FFShaperPlugParamCount };

enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciPlugParamOn, FFOsciPlugParamType, FFOsciPlugParamCount };
enum { FFOsciAutoParamGain, FFOsciAutoParamPitch, FFOsciAutoParamCount };

// TODO this is really wasteful because of the FFMonoBlock alignment
// but it's nice to be able to group stuff for each parameter together
struct FFProcessorAutoParamMemory
{
  int eventPos;
  float scalar;
  FFMonoBlock dense;
};

struct FFPluginProcessors
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPluginProcessors);

  std::array<FFOsciProcessor, FF_OSCI_COUNT> osci;
  std::array<FFShaperProcessor, FF_SHAPER_COUNT> shaper;
};

template <class T>
struct FFAutoParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFAutoParamMemory);

  std::array<std::array<T, FFOsciAutoParamCount>, FF_OSCI_COUNT> osci;
  std::array<std::array<T, FFShaperAutoParamCount>, FF_SHAPER_COUNT> shaper;
};

struct FFPlugParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPlugParamMemory);

  std::array<std::array<float, FFOsciPlugParamCount>, FF_OSCI_COUNT> osci;
  std::array<std::array<float, FFShaperPlugParamCount>, FF_SHAPER_COUNT> shaper;
};

// both processor/controller
struct FFPluginMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPluginMemory);

  FFPlugParamMemory plugParam;
  FFAutoParamMemory<float> scalarAutoParam;
};

struct FFProcessorMemory:
FFPluginMemory
{
  typedef FFPluginMemory PluginMemory;
  static constexpr int BlockSize = FF_BLOCK_SIZE;
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFProcessorMemory);

  FFStereoBlock masterOut;
  FFAutoParamMemory<int> eventPosAutoParam;
  FFAutoParamMemory<FFMonoBlock> denseAutoParam;

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
