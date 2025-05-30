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

using namespace juce::dsp;

static int constexpr EffectOversampleFactor = 2;
static int constexpr EffectOversampleTimes = 1 << EffectOversampleFactor;
static int constexpr EffectFixedBlockOversamples = FBFixedBlockSamples * EffectOversampleTimes;

FFEffectProcessor::
FFEffectProcessor() :
_oversampler(
  2, EffectOversampleFactor,
  Oversampling<float>::filterHalfBandPolyphaseIIR, false, false)
{
  _oversampler.initProcessing(FBFixedBlockSamples);
}

void
FFEffectProcessor::BeginVoice(int graphIndex, FBModuleProcState& state)
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
  bool oversample = topo.NormalizedToBoolFast(FFEffectParam::Oversample, oversampleNorm);
  _oversampleTimes = oversample ? EffectOversampleTimes : 1;
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    bool blockActive = graphIndex == -1 || graphIndex == i;
    _kind[i] = !blockActive? FFEffectKind::Off: topo.NormalizedToListFast<FFEffectKind>(FFEffectParam::Kind, kindNorm[i].Voice()[voice]);
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

  FBSArray<float, EffectFixedBlockOversamples> feedbackPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> distAmtPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> distMixPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> distBiasPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> distDrivePlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> stVarResPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> stVarFreqPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> stVarGainPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> stVarKeyTrkPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> combKeyTrkPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> combResMinPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> combResPlusPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> combFreqMinPlain;
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> combFreqPlusPlain;
  
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    for (int i = 0; i < FFEffectBlockCount; i++)
    {
      feedbackPlain.Store(s, topo.NormalizedToIdentityFast(FFEffectParam::Feedback, feedbackNorm, s));
      if (_kind[i] == FFEffectKind::StVar)
      {
        stVarFreqPlain[i].Store(s, topo.NormalizedToLog2Fast(FFEffectParam::StVarFreq, stVarFreqNorm[i].Voice()[voice], s));
        stVarResPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::StVarRes, stVarResNorm[i].Voice()[voice], s));
        stVarGainPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::StVarGain, stVarGainNorm[i].Voice()[voice], s));
        stVarKeyTrkPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::StVarKeyTrak, stVarKeyTrkNorm[i].Voice()[voice], s));
      }
      else if (_kind[i] == FFEffectKind::Comb)
      {
        combKeyTrkPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombKeyTrk, combKeyTrkNorm[i].Voice()[voice], s));
        combFreqMinPlain[i].Store(s, topo.NormalizedToLog2Fast(FFEffectParam::CombFreqMin, combFreqMinNorm[i].Voice()[voice], s));
        combFreqPlusPlain[i].Store(s, topo.NormalizedToLog2Fast(FFEffectParam::CombFreqPlus, combFreqPlusNorm[i].Voice()[voice], s));
        combResMinPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::CombResMin, combResMinNorm[i].Voice()[voice], s));
        combResPlusPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::CombResPlus, combResPlusNorm[i].Voice()[voice], s));
      }
      else if (_kind[i] == FFEffectKind::Clip || _kind[i] == FFEffectKind::Fold || _kind[i] == FFEffectKind::Skew)
      {
        distAmtPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::DistAmt, distAmtNorm[i].Voice()[voice], s));
        distMixPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::DistMix, distMixNorm[i].Voice()[voice], s));
        distBiasPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::DistBias, distBiasNorm[i].Voice()[voice], s));
        distDrivePlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::DistDrive, distDriveNorm[i].Voice()[voice], s));
      }
      else
        assert(_kind[i] == FFEffectKind::Off);
    }

  if (_oversampleTimes != 1)
  {
    feedbackPlain.UpsampleStretch<EffectOversampleTimes>();
    for (int i = 0; i < FFEffectBlockCount; i++)
    {
      if (_kind[i] == FFEffectKind::StVar)
      {
        stVarFreqPlain[i].UpsampleStretch<EffectOversampleTimes>();
        stVarResPlain[i].UpsampleStretch<EffectOversampleTimes>();
        stVarGainPlain[i].UpsampleStretch<EffectOversampleTimes>();
        stVarKeyTrkPlain[i].UpsampleStretch<EffectOversampleTimes>();
      }
      else if (_kind[i] == FFEffectKind::Comb)
      {
        combKeyTrkPlain[i].UpsampleStretch<EffectOversampleTimes>();
        combFreqMinPlain[i].UpsampleStretch<EffectOversampleTimes>();
        combFreqPlusPlain[i].UpsampleStretch<EffectOversampleTimes>();
        combResMinPlain[i].UpsampleStretch<EffectOversampleTimes>();
        combResPlusPlain[i].UpsampleStretch<EffectOversampleTimes>();
      }
      else if (_kind[i] == FFEffectKind::Clip || _kind[i] == FFEffectKind::Fold || _kind[i] == FFEffectKind::Skew)
      {
        distAmtPlain[i].UpsampleStretch<EffectOversampleTimes>();
        distMixPlain[i].UpsampleStretch<EffectOversampleTimes>();
        distBiasPlain[i].UpsampleStretch<EffectOversampleTimes>();
        distDrivePlain[i].UpsampleStretch<EffectOversampleTimes>();
      }
      else
        assert(_kind[i] == FFEffectKind::Off);
    }
  }

  AudioBlock<float> oversampledBlock = {};
  FBSArray2<float, EffectFixedBlockOversamples, 2> oversampled;
  if (_oversampleTimes == 1)
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        oversampled[c].Store(s, input[c].Load(s));
  else
  {
    float const* audioIn[2] = {};
    audioIn[0] = input[0].Ptr(0);
    audioIn[1] = input[1].Ptr(0);
    AudioBlock<float const> inputBlock(audioIn, 2, 0, FBFixedBlockSamples);
    oversampledBlock = _oversampler.processSamplesUp(inputBlock);
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < EffectFixedBlockOversamples; s++)
        oversampled[c].Set(s, oversampledBlock.getSample(c, s));
  }

  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
    for (int i = 0; i < FFEffectBlockCount; i++)
      if (_kind[i] == FFEffectKind::Clip)
        for (int c = 0; c < 2; c++)
        {
          auto mix = distMixPlain[i].Load(s);
          auto drive = distDrivePlain[i].Load(s);
          auto inSample = oversampled[c].Load(s);
          auto shapedSample = inSample * drive;
          switch (_clipMode[i])
          {
          case FFEffectClipMode::TanH: shapedSample = xsimd::tanh(shapedSample); break;
          case FFEffectClipMode::Hard: shapedSample = xsimd::clip(shapedSample, FBBatch<float>(-1.0f), FBBatch<float>(1.0f)); break;
          default: break;
          }
          auto mixedSample = (1.0f - mix) * inSample + mix * shapedSample;
          oversampled[c].Store(s, mixedSample);
        }

  if(_oversampleTimes == 1)
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        output[c].Store(s, oversampled[c].Load(s));
  else
  {
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < EffectFixedBlockOversamples; s++)
        oversampledBlock.setSample(c, s, oversampled[c].Get(s));
    float* audioOut[2] = {};
    audioOut[0] = output[0].Ptr(0);
    audioOut[1] = output[1].Ptr(0);
    AudioBlock<float> outputBlock(audioOut, 2, 0, FBFixedBlockSamples);
    _oversampler.processSamplesDown(outputBlock);
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