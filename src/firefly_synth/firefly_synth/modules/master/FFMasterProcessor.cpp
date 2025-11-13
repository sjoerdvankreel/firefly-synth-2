#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_synth/modules/master/FFMasterProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
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
  auto& gNoteDspState = procState->dsp.global.gNote;
  auto const& params = procState->param.global.master[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Master];

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
    dspState.outputLastKeyPitch.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixRaw.entries[(int)FBNoteMatrixEntry::LastKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputLowKeyPitch.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixRaw.entries[(int)FBNoteMatrixEntry::LowKeyKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputHighKeyPitch.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixRaw.entries[(int)FBNoteMatrixEntry::HighKeyKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputLowVeloPitch.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixRaw.entries[(int)FBNoteMatrixEntry::LowVeloKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputHighVeloPitch.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixRaw.entries[(int)FBNoteMatrixEntry::HighVeloKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputLastKeyPitchSmth.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixSmth.entries[(int)FBNoteMatrixEntry::LastKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputLowKeyPitchSmth.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixSmth.entries[(int)FBNoteMatrixEntry::LowKeyKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputHighKeyPitchSmth.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixSmth.entries[(int)FBNoteMatrixEntry::HighKeyKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputLowVeloPitchSmth.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixSmth.entries[(int)FBNoteMatrixEntry::LowVeloKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    dspState.outputHighVeloPitchSmth.Store(s, xsimd::clip(gNoteDspState.outputNoteMatrixSmth.entries[(int)FBNoteMatrixEntry::HighVeloKey].Load(s) + normBendAmtBP, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
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