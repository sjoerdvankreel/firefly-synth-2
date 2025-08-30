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
  auto const& params = procState->param.global.master[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Master];

  float tuningModeNorm = procState->param.global.master[0].block.tuningMode[0].Value();
  dspState.mtsEspOn = MTS_HasMaster(dspState.mtsClient);
  dspState.tuningMode = topo.NormalizedToListFast<FFMasterTuningMode>(FFMasterParam::TuningMode, tuningModeNorm);

  for (int i = 0; i < FFMasterAuxCount; i++)
    params.acc.aux[i].Global().CV().CopyTo(dspState.outputAux[i]);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->global.master[0];
  exchangeDSP.active = true;
  auto& exchangeParams = exchangeToGUI->param.global.master[0];
  for (int i = 0; i < FFMasterAuxCount; i++)
    exchangeParams.acc.aux[i] = params.acc.aux[i].Global().Last();
}