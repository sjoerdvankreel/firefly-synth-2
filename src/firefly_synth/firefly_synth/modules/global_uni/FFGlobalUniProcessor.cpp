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

FFGlobalUniProcessor::
FFGlobalUniProcessor()
{
  for (int i = 0; i < (int)FFGlobalUniTarget::Count; i++)
    _prevRandSeedNorm[i] = -1.0f;
}

void
FFGlobalUniProcessor::BeginVoice(int voice)
{
  if (_voiceCount < 2)
    return;
  for (int i = 0; i < (int)FFGlobalUniTarget::Count; i++)
  {
    if (_mode[i] != FFGlobalUniMode::Auto)
      continue;
    _voiceRandState[voice][i] = _randStream[i].NextScalar();
  }
}

void 
FFGlobalUniProcessor::BeginBlock(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.globalUni[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GlobalUni];
  float countNorm = params.block.voiceCount[0].Value();
  _voiceCount = topo.NormalizedToDiscreteFast((int)FFGlobalUniParam::VoiceCount, countNorm);
  for (int i = 0; i < (int)FFGlobalUniTarget::Count; i++)
  {
    _mode[i] = topo.NormalizedToListFast<FFGlobalUniMode>(FFGlobalUniParam::Mode, params.block.mode[i].Value());
    if (_mode[i] != FFGlobalUniMode::Auto)
      continue;

    _randSeedNorm[i] = params.block.autoRandSeed[i].Value();
    _randFree[i] = topo.NormalizedToBoolFast(FFGlobalUniParam::AutoRandFree, params.block.autoRandFree[i].Value());
    if (_randSeedNorm[i] != _prevRandSeedNorm[i] || !_randFree[i])
    {
      _prevRandSeedNorm[i] = _randSeedNorm[i];
      _randStream[i] = FFParkMillerPRNG(_randSeedNorm[i]);
    }
  }
}

void
FFGlobalUniProcessor::Process(FBModuleProcState& state)
{
  if (_voiceCount < 2)
    return;

  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.globalUni[0];

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->global.globalUni[0];
  exchangeDSP.boolIsActive = 1;

  auto& exchangeParams = exchangeToGUI->param.global.globalUni[0];
  for (int i = 0; i < (int)FFGlobalUniTarget::Count; i++)
  {
    exchangeParams.acc.autoRand[i] = params.acc.autoRand[i].Global().Last();
    exchangeParams.acc.autoSkew[i] = params.acc.autoSkew[i].Global().Last();
    exchangeParams.acc.autoSpread[i] = params.acc.autoSpread[i].Global().Last();
  }
  for (int i = 0; i < FFGlobalUniMaxCount; i++)
  {
    exchangeParams.acc.manualVoiceCoarse[i] = params.acc.manualVoiceCoarse[i].Global().Last();
    exchangeParams.acc.manualVoiceFine[i] = params.acc.manualVoiceFine[i].Global().Last();
    exchangeParams.acc.manualVMixAmp[i] = params.acc.manualVMixAmp[i].Global().Last();
    exchangeParams.acc.manualVMixBal[i] = params.acc.manualVMixBal[i].Global().Last();
    exchangeParams.acc.manualOscPhaseOffset[i] = params.acc.manualOscPhaseOffset[i].Global().Last();
    exchangeParams.acc.manualOscPan[i] = params.acc.manualOscPan[i].Global().Last();
    exchangeParams.acc.manualOscGain[i] = params.acc.manualOscGain[i].Global().Last();
    exchangeParams.acc.manualOscFine[i] = params.acc.manualOscFine[i].Global().Last();
    exchangeParams.acc.manualOscCoarse[i] = params.acc.manualOscCoarse[i].Global().Last();
    exchangeParams.acc.manualLFOPhaseOffset[i] = params.acc.manualLFOPhaseOffset[i].Global().Last();
    exchangeParams.acc.manualLFORate[i] = params.acc.manualLFORate[i].Global().Last();
    exchangeParams.acc.manualLFOMin[i] = params.acc.manualLFOMin[i].Global().Last();
    exchangeParams.acc.manualLFOMax[i] = params.acc.manualLFOMax[i].Global().Last();
    exchangeParams.acc.manualLFOSkewAX[i] = params.acc.manualLFOSkewAX[i].Global().Last();
    exchangeParams.acc.manualLFOSkewAY[i] = params.acc.manualLFOSkewAY[i].Global().Last();
    exchangeParams.acc.manualVFXParamA[i] = params.acc.manualVFXParamA[i].Global().Last();
    exchangeParams.acc.manualVFXParamB[i] = params.acc.manualVFXParamB[i].Global().Last();
    exchangeParams.acc.manualVFXParamC[i] = params.acc.manualVFXParamC[i].Global().Last();
    exchangeParams.acc.manualVFXParamD[i] = params.acc.manualVFXParamD[i].Global().Last();
    exchangeParams.acc.manualEnvOffset[i] = params.acc.manualEnvOffset[i].Global().Last();
    exchangeParams.acc.manualEnvStretch[i] = params.acc.manualEnvStretch[i].Global().Last();
    exchangeParams.acc.manualEchoFade[i] = params.acc.manualEchoFade[i].Global().Last();
    exchangeParams.acc.manualEchoExtend[i] = params.acc.manualEchoExtend[i].Global().Last();
    exchangeParams.acc.manualEchoTapLevel[i] = params.acc.manualEchoTapLevel[i].Global().Last();
    exchangeParams.acc.manualEchoTapDelay[i] = params.acc.manualEchoTapDelay[i].Global().Last();
    exchangeParams.acc.manualEchoTapBal[i] = params.acc.manualEchoTapBal[i].Global().Last();
    exchangeParams.acc.manualEchoFdbkDelay[i] = params.acc.manualEchoFdbkDelay[i].Global().Last();
    exchangeParams.acc.manualEchoFdbkMix[i] = params.acc.manualEchoFdbkMix[i].Global().Last();
    exchangeParams.acc.manualEchoFdbkAmt[i] = params.acc.manualEchoFdbkAmt[i].Global().Last();
    exchangeParams.acc.manualEchoReverbMix[i] = params.acc.manualEchoReverbMix[i].Global().Last();
    exchangeParams.acc.manualEchoReverbSize[i] = params.acc.manualEchoReverbSize[i].Global().Last();
    exchangeParams.acc.manualEchoReverbDamp[i] = params.acc.manualEchoReverbDamp[i].Global().Last();
  }
}