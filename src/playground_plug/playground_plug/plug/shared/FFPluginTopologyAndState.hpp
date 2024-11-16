#pragma once

#include <playground_plug/base/shared/FBPluginTopology.hpp>
#include <playground_plug/plug/shared/FFPluginBlocks.hpp>
#include <playground_plug/plug/shared/FFPluginConfiguration.hpp>

#include <array>

enum { FFModuleOscillator, FFModuleWaveShaper, FFModuleCount };
enum { FFWaveShaperDiscreteParamOn, FFWaveShaperDiscreteParamCount };
enum { FFWaveShaperContinuousParamGain, FFWaveShaperContinuousParamCount };
enum { FFOscillatorDiscreteParamCount };
enum { FFOscillatorContinuousParamGain, FFOscillatorContinuousParamPitch, FFOscillatorContinuousParamCount };

std::unique_ptr<FBPluginStaticTopology>
FFCreateStaticTopology();

template <class T>
struct FFPluginParameterMemory
{
};

template <class T>
struct FFWaveShaperParameters
{
  T gain;
};

template <class T>
struct FFPluginParameters
{
  std::array<FFOscillatorParameters<T>, FF_OSCILLATOR_COUNT> oscillator;
  std::array<FFWaveShaperParameters<T>, FF_WAVE_SHAPER_COUNT> waveShaper;
};

typedef FFPluginParameters<float> FFControllerParameters;
typedef FFPluginParameters<FFCVBlock> FFProcessorParameters;