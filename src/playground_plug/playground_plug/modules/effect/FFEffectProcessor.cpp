#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>
#include <playground_plug/modules/effect/FFEffectProcessor.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

void
FFEffectProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.effect[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Effect];
  
  auto const& kindNorm = params.block.kind;
  auto const& clipModeNorm = params.block.clipMode;
  auto const& foldModeNorm = params.block.foldMode;
  auto const& skewModeNorm = params.block.skewMode;
  auto const& stVarModeNorm = params.block.stVarMode;
  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& oversampleNorm = params.block.oversample[0].Voice()[voice];

  _type = topo.NormalizedToListFast<FFEffectType>(FFEffectParam::Type, typeNorm);
  _oversample = topo.NormalizedToBoolFast(FFEffectParam::Oversample, oversampleNorm);
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    _kind[i] = topo.NormalizedToListFast<FFEffectKind>(FFEffectParam::Kind, kindNorm[i].Voice()[voice]);
    _clipMode[i] = topo.NormalizedToListFast<FFEffectClipMode>(FFEffectParam::ClipMode, clipModeNorm[i].Voice()[voice]);
    _foldMode[i] = topo.NormalizedToListFast<FFEffectFoldMode>(FFEffectParam::FoldMode, foldModeNorm[i].Voice()[voice]);
    _skewMode[i] = topo.NormalizedToListFast<FFEffectSkewMode>(FFEffectParam::SkewMode, skewModeNorm[i].Voice()[voice]);
    _stVarMode[i] = topo.NormalizedToListFast<FBCytomicFilterMode>(FFEffectParam::StVarMode, stVarModeNorm[i].Voice()[voice]);
  }
}

void
FFEffectProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.effect[state.moduleSlot].output;
  auto const& input = voiceState.effect[state.moduleSlot].input;
  auto& oversampled = voiceState.effect[state.moduleSlot].oversampled;

  if (_type == FFEffectType::Off)
  {
    input.CopyTo(output);
    return;
  }

  auto const& procParams = procState->param.voice.effect[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Effect];
  auto const& distAmtNorm = procParams.acc.distAmt;
  auto const& distMixNorm = procParams.acc.distMix;
  auto const& distBiasNorm = procParams.acc.distBias;
  auto const& distDriveNorm = procParams.acc.distDrive;
  auto const& stVarResNorm = procParams.acc.stVarRes;
  auto const& stVarFreqNorm = procParams.acc.stVarFreq;
  auto const& stVarGainNorm = procParams.acc.stVarGain;
  auto const& stVarKeyTrkNorm = procParams.acc.stVarKeyTrk;
  auto const& combKeyTrkNorm = procParams.acc.combKeyTrk;
  auto const& combResMinNorm = procParams.acc.combResMin;
  auto const& combResPlusNorm = procParams.acc.combResPlus;
  auto const& combFreqMinNorm = procParams.acc.combFreqMin;
  auto const& combFreqPlusNorm = procParams.acc.combFreqPlus;
  auto const& feedbackNorm = procParams.acc.feedback[0].Voice()[voice];

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->voice[voice].effect[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = -1;

  auto& exchangeParams = exchangeToGUI->param.voice.effect[state.moduleSlot];
  exchangeParams.acc.feedback[0][voice] = feedbackNorm.Last();
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    exchangeParams.acc.distAmt[i][voice] = distAmtNorm[i].Voice()[voice].Last();
    exchangeParams.acc.distMix[i][voice] = distMixNorm[i].Voice()[voice].Last();
    exchangeParams.acc.distBias[i][voice] = distBiasNorm[i].Voice()[voice].Last();
    exchangeParams.acc.distDrive[i][voice] = distDriveNorm[i].Voice()[voice].Last();
    exchangeParams.acc.stVarRes[i][voice] = stVarResNorm[i].Voice()[voice].Last();
    exchangeParams.acc.stVarFreq[i][voice] = stVarFreqNorm[i].Voice()[voice].Last();
    exchangeParams.acc.stVarGain[i][voice] = stVarGainNorm[i].Voice()[voice].Last();
    exchangeParams.acc.stVarKeyTrk[i][voice] = stVarKeyTrkNorm[i].Voice()[voice].Last();
    exchangeParams.acc.combKeyTrk[i][voice] = combKeyTrkNorm[i].Voice()[voice].Last();
    exchangeParams.acc.combResMin[i][voice] = combResMinNorm[i].Voice()[voice].Last();
    exchangeParams.acc.combResPlus[i][voice] = combResPlusNorm[i].Voice()[voice].Last();
    exchangeParams.acc.combFreqMin[i][voice] = combFreqMinNorm[i].Voice()[voice].Last();
    exchangeParams.acc.combFreqPlus[i][voice] = combFreqPlusNorm[i].Voice()[voice].Last();
  }
}