#pragma once

#include <playground_plug/base/shared/FBPluginTopology.hpp>
#include <playground_plug/plug/dsp/FFOscillatorProcessor.hpp>
#include <playground_plug/plug/dsp/FFWaveShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginBlocks.hpp>
#include <playground_plug/plug/shared/FFPluginConfiguration.hpp>

#include <array>

enum { FFModuleOscillator, FFModuleWaveShaper, FFModuleCount };
enum { FFWaveShaperParamOn, FFWaveShaperParamGain, FFWaveShaperParamCount };
enum { FFOscillatorParamGain, FFOscillatorParamPitch, FFOscillatorParamCount };

std::unique_ptr<FBPluginStaticTopology>
FFCreateStaticTopology();

struct FFPluginProcessors
{
  std::array<FFOscillatorProcessor, FF_OSCILLATOR_COUNT> oscillator;
  std::array<FFWaveShaperProcessor, FF_WAVE_SHAPER_COUNT> waveShaper;
};

template <class T>
struct FFPluginParameterMemory
{
  std::array<std::array<T, FFOscillatorParamCount>, FF_OSCILLATOR_COUNT> oscillator;
  std::array<std::array<T, FFWaveShaperParamCount>, FF_WAVE_SHAPER_COUNT> waveShaper;
};

typedef FFPluginParameterMemory<float> FFControllerParameterMemory;
typedef FFPluginParameterMemory<FFCVBlock> FFProcessorParameterMemory;

struct FFPluginProcessorBlock
{
  FFAudioBlock masterOutput;
  FFProcessorParameterMemory parameterMemory;
  std::array<FFAudioBlock, FF_OSCILLATOR_COUNT> oscillatorAudioOutput;
  std::array<FFAudioBlock, FF_WAVE_SHAPER_COUNT> waveShaperAudioInput;
  std::array<FFAudioBlock, FF_WAVE_SHAPER_COUNT> waveShaperAudioOutput;
};