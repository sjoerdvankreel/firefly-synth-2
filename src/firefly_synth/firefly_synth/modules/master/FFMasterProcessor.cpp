#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_synth/modules/master/FFMasterProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBTune.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <libMTSClient.h>
#include <xsimd/xsimd.hpp>

void
FFMasterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& dspState = procState->dsp.global.master;
  auto const& gNoteDspState = procState->dsp.global.gNote;
  auto const& params = procState->param.global.master[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Master];
  auto const& gNoteRaw = gNoteDspState.outputNoteMatrixRaw.entries;
  auto const& gNoteSmth = gNoteDspState.outputNoteMatrixSmth.entries;

  params.acc.modWheel[0].Global().CV().CopyTo(dspState.outputMod);
  params.acc.pitchBend[0].Global().CV().CopyTo(dspState.outputPBRaw);
  for (int i = 0; i < FFMasterAuxCount; i++)
    params.acc.aux[i].Global().CV().CopyTo(dspState.outputAux[i]);

  float bendRangeNorm = params.block.pitchBendRange[0].Value();
  float bendTargetNorm = params.block.pitchBendTarget[0].Value();
  dspState.mtsEspOn = MTS_HasMaster(dspState.mtsClient);
  dspState.bendTarget = topo.NormalizedToListFast<FFMasterPitchBendTarget>(FFMasterParam::PitchBendTarget, bendTargetNorm);
  
  auto const& bendAmountNorm = params.acc.pitchBend[0].Global();
  float bendRangeSemis = (float)topo.NormalizedToDiscreteFast(FFMasterParam::PitchBendRange, bendRangeNorm);
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    FBBatch<float> bendAmountPlain = topo.NormalizedToLinearFast(FFMasterParam::PitchBend, bendAmountNorm, s);
    dspState.bendAmountInSemis.Store(s, bendAmountPlain * bendRangeSemis);
    FBBatch<float> normBendAmtBP = FBToBipolar(bendAmountNorm.CV().Load(s)) * bendRangeSemis / 127.0f;
    dspState.outputPB.Store(s, FBToUnipolar(normBendAmtBP));
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float pb = dspState.bendAmountInSemis.Get(s);
    float lastKeyRaw = gNoteRaw[(int)FBNoteMatrixEntry::LastKey].Get(s) * 127.0f;
    float highKeyKeyRaw = gNoteRaw[(int)FBNoteMatrixEntry::HighKeyKey].Get(s) * 127.0f;
    float lowKeyKeyRaw = gNoteRaw[(int)FBNoteMatrixEntry::LowKeyKey].Get(s) * 127.0f;
    float highVeloKeyRaw = gNoteRaw[(int)FBNoteMatrixEntry::HighVeloKey].Get(s) * 127.0f;
    float lowVeloKeyRaw = gNoteRaw[(int)FBNoteMatrixEntry::LowVeloKey].Get(s) * 127.0f;
    float lastKeySmth = gNoteSmth[(int)FBNoteMatrixEntry::LastKey].Get(s) * 127.0f;
    float highKeyKeySmth = gNoteSmth[(int)FBNoteMatrixEntry::HighKeyKey].Get(s) * 127.0f;
    float lowKeyKeySmth = gNoteSmth[(int)FBNoteMatrixEntry::LowKeyKey].Get(s) * 127.0f;
    float highVeloKeySmth = gNoteSmth[(int)FBNoteMatrixEntry::HighVeloKey].Get(s) * 127.0f;
    float lowVeloKeySmth = gNoteSmth[(int)FBNoteMatrixEntry::LowVeloKey].Get(s) * 127.0f;
    if (procState->dsp.global.settings.tuning && procState->dsp.global.settings.tuneMasterPB)
    {
      lastKeyRaw += pb;
      highKeyKeyRaw += pb;
      lowKeyKeyRaw += pb;
      highVeloKeyRaw += pb;
      lowVeloKeyRaw += pb;
      lastKeySmth += pb;
      highKeyKeySmth += pb;
      lowKeyKeySmth += pb;
      highVeloKeySmth += pb;
      lowVeloKeySmth += pb;
    }
    if (procState->dsp.global.settings.tuning && procState->dsp.global.settings.tuneMasterMatrix)
    {
      lastKeyRaw = FBTuneReal(procState->dsp.global.master.mtsClient, lastKeyRaw, -1);
      highKeyKeyRaw = FBTuneReal(procState->dsp.global.master.mtsClient, highKeyKeyRaw, -1);
      lowKeyKeyRaw = FBTuneReal(procState->dsp.global.master.mtsClient, lowKeyKeyRaw, -1);
      highVeloKeyRaw = FBTuneReal(procState->dsp.global.master.mtsClient, highVeloKeyRaw, -1);
      lowVeloKeyRaw = FBTuneReal(procState->dsp.global.master.mtsClient, lowVeloKeyRaw, -1);
      lastKeySmth = FBTuneReal(procState->dsp.global.master.mtsClient, lastKeySmth, -1);
      highKeyKeySmth = FBTuneReal(procState->dsp.global.master.mtsClient, highKeyKeySmth, -1);
      lowKeyKeySmth = FBTuneReal(procState->dsp.global.master.mtsClient, lowKeyKeySmth, -1);
      highVeloKeySmth = FBTuneReal(procState->dsp.global.master.mtsClient, highVeloKeySmth, -1);
      lowVeloKeySmth = FBTuneReal(procState->dsp.global.master.mtsClient, lowVeloKeySmth, -1);
    }
    if (!procState->dsp.global.settings.tuning || !procState->dsp.global.settings.tuneMasterPB)
    {
      lastKeyRaw += pb;
      highKeyKeyRaw += pb;
      lowKeyKeyRaw += pb;
      highVeloKeyRaw += pb;
      lowVeloKeyRaw += pb;
      lastKeySmth += pb;
      highKeyKeySmth += pb;
      lowKeyKeySmth += pb;
      highVeloKeySmth += pb;
      lowVeloKeySmth += pb;
    }
    dspState.outputLastKeyPitch.Set(s, std::clamp(lastKeyRaw, 0.0f, 127.0f) / 127.0f);
    dspState.outputHighKeyPitch.Set(s, std::clamp(highKeyKeyRaw, 0.0f, 127.0f) / 127.0f);
    dspState.outputLowKeyPitch.Set(s, std::clamp(lowKeyKeyRaw, 0.0f, 127.0f) / 127.0f);
    dspState.outputHighVeloPitch.Set(s, std::clamp(highVeloKeyRaw, 0.0f, 127.0f) / 127.0f);
    dspState.outputLowVeloPitch.Set(s, std::clamp(lowVeloKeyRaw, 0.0f, 127.0f) / 127.0f);
    dspState.outputLastKeyPitchSmth.Set(s, std::clamp(lastKeySmth, 0.0f, 127.0f) / 127.0f);
    dspState.outputHighKeyPitchSmth.Set(s, std::clamp(highKeyKeySmth, 0.0f, 127.0f) / 127.0f);
    dspState.outputLowKeyPitchSmth.Set(s, std::clamp(lowKeyKeySmth, 0.0f, 127.0f) / 127.0f);
    dspState.outputHighVeloPitchSmth.Set(s, std::clamp(highVeloKeySmth, 0.0f, 127.0f) / 127.0f);
    dspState.outputLowVeloPitchSmth.Set(s, std::clamp(lowVeloKeySmth, 0.0f, 127.0f) / 127.0f);
  }

  // special source for pitchtracking global filters, not visible in the matrix
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float pb = dspState.bendAmountInSemis.Get(s);
    float lastKeyRaw = gNoteRaw[(int)FBNoteMatrixEntry::LastKey].Get(s) * 127.0f;
    if (procState->dsp.global.settings.tuning && procState->dsp.global.settings.tuneMasterPB)
      lastKeyRaw += pb;
    if (procState->dsp.global.settings.tuning)
      lastKeyRaw = FBTuneReal(procState->dsp.global.master.mtsClient, lastKeyRaw, -1);
    if (!procState->dsp.global.settings.tuning || !procState->dsp.global.settings.tuneMasterPB)
      lastKeyRaw += pb;
    dspState.outputLastKeyPitchTunedRaw.Set(s, std::clamp(lastKeyRaw, 0.0f, 127.0f) / 127.0f);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->global.master[0];
  exchangeDSP.boolIsActive = 1;
  
  auto& exchangeParams = exchangeToGUI->param.global.master[0];
  exchangeParams.acc.modWheel[0] = params.acc.modWheel[0].Global().Last();
  exchangeParams.acc.pitchBend[0] = params.acc.pitchBend[0].Global().Last();
  for (int i = 0; i < FFMasterAuxCount; i++)
    exchangeParams.acc.aux[i] = params.acc.aux[i].Global().Last();
}