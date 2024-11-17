#pragma once

#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginBlocks.hpp>
#include <playground_plug/plug/shared/FFPluginConfig.hpp>

#include <array>

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };
enum { FFOsciParamGain, FFOsciParamPitch, FFOsciParamCount };
enum { FFShaperParamOn, FFShaperParamGain, FFShaperParamCount };

std::unique_ptr<FBStaticTopo>
FFMakeTopo();

struct FFPluginProcessors
{
  std::array<FFOsciProcessor, FF_OSCI_COUNT> osci;
  std::array<FFShaperProcessor, FF_SHAPER_COUNT> shaper;
};

template <class T>
struct FFParamMemory
{
  std::array<std::array<T, FFOsciParamCount>, FF_OSCI_COUNT> osci;
  std::array<std::array<T, FFShaperParamCount>, FF_SHAPER_COUNT> shaper;
};

typedef FFParamMemory<float> FFControllerParamMemory;
typedef FFParamMemory<FFMonoBlock> FFProcessorParamMemory;

struct FFProcessorBlock
{
  float sampleRate;
  FFProcessorParamMemory paramMemory;
  std::array<FFStereoBlock, FF_OSCI_COUNT> osciOutput;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperInput;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperOutput;
};