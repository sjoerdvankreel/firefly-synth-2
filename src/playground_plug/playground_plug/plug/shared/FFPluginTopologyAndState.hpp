#pragma once

#include <playground_plug/base/shared/FBPluginTopology.hpp>
#include <playground_plug/plug/shared/FFPluginBlocks.hpp>
#include <playground_plug/plug/shared/FFPluginConfiguration.hpp>

#include <array>

enum { FFModuleOscillator, FFModuleWaveShaper, FFModuleCount };
enum { FFWaveShaperParamOn, FFWaveShaperParamGain, FFWaveShaperParamCount };
enum { FFOscillatorParamGain, FFOscillatorParamPitch, FFOscillatorParamCount };

std::unique_ptr<FBPluginStaticTopology>
FFCreateStaticTopology();

template <class T>
struct FFPluginParameterMemory
{
  std::array<std::array<T, FFOscillatorParamCount>, FF_OSCILLATOR_COUNT> oscillator;
  std::array<std::array<T, FFWaveShaperParamCount>, FF_WAVE_SHAPER_COUNT> waveShaper;
};

typedef FFPluginParameterMemory<float> FFControllerParameterMemory;
typedef FFPluginParameterMemory<FFCVBlock> FFProcessorParameterMemory;