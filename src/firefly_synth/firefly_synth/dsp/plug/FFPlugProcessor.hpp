#pragma once

#include <firefly_synth/dsp/shared/FFMarsagliaPRNG.hpp>
#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>

#include <firefly_base/dsp/plug/FBPlugProcessor.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

struct FFProcState;
struct FBRuntimeTopo;
struct FFExchangeState;
struct FBPlugOutputBlock;

class IFBHostDSPContext;

class FFPlugProcessor final:
public IFBPlugProcessor
{
  float const _sampleRate;
  FBRuntimeTopo const* const _topo;
  FFProcState* const _procState;
  FFExchangeState* const _exchangeState;

  // global uni
  std::int64_t _voiceGroupId = {};

  // on note random
  FFParkMillerPRNG _onNoteRandomUni = {};
  FFMarsagliaPRNG<false> _onNoteRandomNorm = {};

  // connect to manual-click button, sick of dealing with this
  bool _prevFlushAudioToggle = false;

  FBModuleProcState MakeModuleState(FBPlugInputBlock const& input);
  FBModuleProcState MakeModuleVoiceState(FBPlugInputBlock const& input, int voice);
  void ProcessGEcho(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout);
  void ApplyGlobalModulation(FBModuleProcState& state, FBTopoIndices moduleIndices);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(IFBHostDSPContext* hostContext);

  void ProcessPreVoice(FBPlugInputBlock const& input) override;
  void ProcessVoice(FBPlugInputBlock const& input, int voice, int releaseAt) override;
  void ProcessPostVoice(FBPlugInputBlock const& input, FBPlugOutputBlock& output) override;

  void LeaseVoices(FBPlugInputBlock const& input) override;
  void GetCurrentProcessSettings(FBProcessSettings& settings) const override;
  void AllocOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* procState) override;
};