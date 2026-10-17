#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciProcessor::BeginVoiceSample(
  FBModuleProcState& state, bool graph)
{
  (void)graph;
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];
  float sampleIsLoadedNorm = params.block.sampleIsLoaded[0].Voice()[voice];
  _sampleIsLoaded = topo.NormalizedToBoolFast(FFOsciParam::SampleIsLoaded, sampleIsLoadedNorm);
}

void 
FFOsciProcessor::ProcessSample(
  FBModuleProcState& state,
  FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
  FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain)
{
  state.
  // todo sample
  (void)state;
  (void)basePitchPlain;
  (void)uniDetunePlain;
}