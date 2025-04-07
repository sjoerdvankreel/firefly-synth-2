#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciPhase.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/dsp/shared/FBTrackingPhase.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

struct FBModuleProcState;

struct FFOsciVoiceState final
{
  int note = {};
  float key = {}; // TODO floating key
  FFOsciType type = {};

  bool basicSinOn = {};
  bool basicSawOn = {};
  bool basicTriOn = {};
  bool basicSqrOn = {};

  int dsfDistance = {};
  int dsfOvertones = {};
  FFOsciDSFMode dsfMode = {};
  float dsfBandwidthPlain = {};

  int unisonCount = {};
  bool unisonDetuneHQ = {};
  float unisonOffsetPlain = {};
  float unisonRandomPlain = {};

  bool mod4XOversampling = false;
  std::array<int, FFOsciCount - 1> modSourceUnisonCount = {};
  std::array<FFOsciModAMMode, FFOsciCount - 1> modSourceAMMode = {};
  std::array<FFOsciModFMMode, FFOsciCount - 1> modSourceFMMode = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FBParkMillerPRNG _prng = {};
  FFOsciVoiceState _voiceState = {};
  juce::dsp::Oversampling<float> _oversampling;
  juce::dsp::AudioBlock<float> _oversamplingBuffers = {};
  std::array<FFOsciPhase, FFOsciUnisonMaxCount> _unisonPhases = {};

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};