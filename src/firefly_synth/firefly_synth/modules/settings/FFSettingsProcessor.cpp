#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>
#include <firefly_synth/modules/settings/FFSettingsProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

void
FFSettingsProcessor::Process(FBModuleProcState& state)
{
  // Receive notes switch is needed earlier in the pipeline 
  // (before we are called) so it's handled by FFPlugProcessor directly.
  auto* procState = state.ProcAs<FFProcState>();
  auto& dspState = procState->dsp.global.settings;
  auto const& params = procState->param.global.settings[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Settings];
  dspState.tuning = topo.NormalizedToBoolFast((int)FFSettingsParam::Tuning, params.block.tuning[0].Value());
  dspState.tuneOnNote = topo.NormalizedToBoolFast((int)FFSettingsParam::TuneOnNote, params.block.tuneOnNote[0].Value());
  dspState.tuneMasterPB = topo.NormalizedToBoolFast((int)FFSettingsParam::TuneMasterPB, params.block.tuneMasterPB[0].Value());
  dspState.tuneMasterMatrix = topo.NormalizedToBoolFast((int)FFSettingsParam::TuneMasterMatrix, params.block.tuneMasterMatrix[0].Value());
  dspState.tuneVoiceCoarse = topo.NormalizedToBoolFast((int)FFSettingsParam::TuneVoiceCoarse, params.block.tuneVoiceCoarse[0].Value());
  dspState.tuneVoiceMatrix = topo.NormalizedToBoolFast((int)FFSettingsParam::TuneVoiceMatrix, params.block.tuneVoiceMatrix[0].Value());
}