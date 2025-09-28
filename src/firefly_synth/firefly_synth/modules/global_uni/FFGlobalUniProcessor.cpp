#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void 
FFGlobalUniProcessor::BeginBlock(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.globalUni[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GlobalUni];
  float typeNorm = params.block.type[0].Value();
  float countNorm = params.block.voiceCount[0].Value();
  _type = topo.NormalizedToListFast<FFGlobalUniType>((int)FFGlobalUniParam::Type, typeNorm);
  _voiceCount = topo.NormalizedToDiscreteFast((int)FFGlobalUniParam::VoiceCount, countNorm);
}

void
FFGlobalUniProcessor::Process(FBModuleProcState& state)
{
  if (_type == FFGlobalUniType::Off)
    return;

  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.globalUni[0];

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->global.globalUni[0];
  exchangeDSP.boolIsActive = 1;

  auto& exchangeParams = exchangeToGUI->param.global.globalUni[0];
  for (int i = 0; i < FFGlobalUniMaxCount; i++)
  {
    exchangeParams.acc.fullVoiceCoarse[i] = params.acc.fullVoiceCoarse[i].Global().Last();
    exchangeParams.acc.fullVoiceFine[i] = params.acc.fullVoiceFine[i].Global().Last();
    exchangeParams.acc.fullVMixAmp[i] = params.acc.fullVMixAmp[i].Global().Last();
    exchangeParams.acc.fullVMixBal[i] = params.acc.fullVMixBal[i].Global().Last();
    exchangeParams.acc.fullOscPan[i] = params.acc.fullOscPan[i].Global().Last();
    exchangeParams.acc.fullOscGain[i] = params.acc.fullOscGain[i].Global().Last();
    exchangeParams.acc.fullOscFine[i] = params.acc.fullOscFine[i].Global().Last();
    exchangeParams.acc.fullOscCoarse[i] = params.acc.fullOscCoarse[i].Global().Last();
    exchangeParams.acc.fullLFORate[i] = params.acc.fullLFORate[i].Global().Last();
    exchangeParams.acc.fullLFOMin[i] = params.acc.fullLFOMin[i].Global().Last();
    exchangeParams.acc.fullLFOMax[i] = params.acc.fullLFOMax[i].Global().Last();
    exchangeParams.acc.fullLFOSkewAX[i] = params.acc.fullLFOSkewAX[i].Global().Last();
    exchangeParams.acc.fullLFOSkewAY[i] = params.acc.fullLFOSkewAY[i].Global().Last();
    exchangeParams.acc.fullVFXParamA[i] = params.acc.fullVFXParamA[i].Global().Last();
    exchangeParams.acc.fullVFXParamB[i] = params.acc.fullVFXParamB[i].Global().Last();
    exchangeParams.acc.fullVFXParamC[i] = params.acc.fullVFXParamC[i].Global().Last();
    exchangeParams.acc.fullVFXParamD[i] = params.acc.fullVFXParamD[i].Global().Last();
    exchangeParams.acc.fullEchoTapLevel[i] = params.acc.fullEchoTapLevel[i].Global().Last();
    exchangeParams.acc.fullEchoTapDelay[i] = params.acc.fullEchoTapDelay[i].Global().Last();
    exchangeParams.acc.fullEchoTapBal[i] = params.acc.fullEchoTapBal[i].Global().Last();
    exchangeParams.acc.fullEchoFdbkDelay[i] = params.acc.fullEchoFdbkDelay[i].Global().Last();
    exchangeParams.acc.fullEchoFdbkMix[i] = params.acc.fullEchoFdbkMix[i].Global().Last();
    exchangeParams.acc.fullEchoFdbkAmt[i] = params.acc.fullEchoFdbkAmt[i].Global().Last();
    exchangeParams.acc.fullEchoReverbMix[i] = params.acc.fullEchoReverbMix[i].Global().Last();
    exchangeParams.acc.fullEchoReverbSize[i] = params.acc.fullEchoReverbSize[i].Global().Last();
    exchangeParams.acc.fullEchoReverbDamp[i] = params.acc.fullEchoReverbDamp[i].Global().Last();
  }
}