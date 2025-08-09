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

#include <xsimd/xsimd.hpp>

void
FFMasterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& outputAux = procState->dsp.global.master.outputAux;
  auto const& procParams = procState->param.global.master[state.moduleSlot];

  for (int i = 0; i < FFMasterAuxCount; i++)
    procParams.acc.aux[i].Global().CV().CopyTo(outputAux[i]);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->global.master[0];
  exchangeDSP.active = true;
  auto& exchangeParams = exchangeToGUI->param.global.master[0];
  for (int i = 0; i < FFMasterAuxCount; i++)
    exchangeParams.acc.aux[i] = procParams.acc.aux[i].Global().Last();
}