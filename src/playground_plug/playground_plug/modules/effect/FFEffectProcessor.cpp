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

static inline FBBatch<float>
Sin2(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  return xsimd::sin(in * FBPi + sin1);
}

static inline FBBatch<float>
Cos2(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  return xsimd::cos(in * FBPi + cos1);
}

static inline FBBatch<float>
SinCos(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  return xsimd::sin(in * FBPi + cos1);
}

static inline FBBatch<float>
CosSin(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  return xsimd::cos(in * FBPi + sin1);
}

static inline FBBatch<float>
Sin3(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + sin1);
  return xsimd::sin(in * FBPi + sin2);
}

static inline FBBatch<float>
Cos3(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + cos1);
  return xsimd::cos(in * FBPi + cos2);
}

static inline FBBatch<float>
Sn2Cs(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + cos1);
  return xsimd::sin(in * FBPi + sin2);
}

static inline FBBatch<float>
Cs2Sn(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + sin1);
  return xsimd::cos(in * FBPi + cos2);
}

static inline FBBatch<float>
SnCs2(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + cos1);
  return xsimd::sin(in * FBPi + cos2);
}

static inline FBBatch<float>
CsSn2(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + sin1);
  return xsimd::cos(in * FBPi + sin2);
}

static inline FBBatch<float>
SnCsSn(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + sin1);
  return xsimd::sin(in * FBPi + cos2);
}

static inline FBBatch<float>
CsSnCs(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + cos1);
  return xsimd::cos(in * FBPi + sin2);
}

static inline FBBatch<float>
FoldBack(FBBatch<float> in)
{
  FBSArray<float, FBSIMDFloatCount> out;
  out.Store(0, in);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float x = std::clamp(out.Get(i), -32.0f, 32.0f);
    while (true)
    {
      if (x > 1.0f) x -= 2.0f * (x - 1.0f);
      else if (x < -1.0f) x += 2.0f * (-x - 1.0f);
      else break;
    }
    out.Set(i, x);
  }
  return out.Load(0);
}

FFEffectProcessor::
FFEffectProcessor() :
_oversampler(
  2, EffectOversampleFactor,
  Oversampling<float>::filterHalfBandPolyphaseIIR, false, false)
{
  _oversampler.initProcessing(FBFixedBlockSamples);
}

void
FFEffectProcessor::BeginVoice(bool graph, int graphIndex, FBModuleProcState& state)
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
  auto const& onNorm = params.block.on[0].Voice()[voice];
  auto const& oversampleNorm = params.block.oversample[0].Voice()[voice];

  _key = static_cast<float>(state.voice->event.note.key);
  _on = topo.NormalizedToBoolFast(FFEffectParam::On, onNorm);
  bool oversample = topo.NormalizedToBoolFast(FFEffectParam::Oversample, oversampleNorm);
  _oversampleTimes = !graph && oversample ? EffectOversampleTimes : 1;
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    bool blockActive = !graph || graphIndex == i || graphIndex == FFEffectBlockCount;
    _kind[i] = !blockActive? FFEffectKind::Off: topo.NormalizedToListFast<FFEffectKind>(FFEffectParam::Kind, kindNorm[i].Voice()[voice]);
    _clipMode[i] = topo.NormalizedToListFast<FFEffectClipMode>(FFEffectParam::ClipMode, clipModeNorm[i].Voice()[voice]);
    _foldMode[i] = topo.NormalizedToListFast<FFEffectFoldMode>(FFEffectParam::FoldMode, foldModeNorm[i].Voice()[voice]);
    _skewMode[i] = topo.NormalizedToListFast<FFEffectSkewMode>(FFEffectParam::SkewMode, skewModeNorm[i].Voice()[voice]);
    _stVarMode[i] = topo.NormalizedToListFast<FBCytomicFilterMode>(FFEffectParam::StVarMode, stVarModeNorm[i].Voice()[voice]);
    _stVarFilters[i] = {};
  }
}  

void 
FFEffectProcessor::ProcessStVar(
  int block, float oversampledRate,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray<float, EffectFixedBlockOversamples> const& trackingKeyPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarResPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarFreqPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarGainPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarKeyTrkPlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s ++)
  {
    auto trkk = trackingKeyPlain.Get(s);
    auto res = stVarResPlain[block].Get(s);
    auto freq = stVarFreqPlain[block].Get(s);
    auto gain = stVarGainPlain[block].Get(s);
    auto ktrk = stVarKeyTrkPlain[block].Get(s);
    freq *= std::pow(2.0f, (_key - 60.0f + trkk) / 12.0f * ktrk);
    freq = std::clamp(freq, 20.0f, 20000.0f);
    _stVarFilters[block].Set(_stVarMode[block], oversampledRate, freq, res, gain);
    for(int c = 0; c < 2; c++)
      oversampled[c].Set(s, _stVarFilters[block].Next(c, oversampled[c].Get(s)));
  }
}

void 
FFEffectProcessor::ProcessSkew(
  int block,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  auto invLogHalf = 1.0f / std::log(0.5f);
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
  {
    auto mix = distMixPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    {
      auto inBatch = oversampled[c].Load(s);
      auto shapedBatch = (inBatch + bias) * drive;
      auto signBatch = xsimd::sign(shapedBatch);
      auto expoBatch = xsimd::log(0.01f + distAmtPlain[block].Load(s) * 0.98f) * invLogHalf;
      switch (_skewMode[block])
      {
      case FFEffectSkewMode::Bi:
        shapedBatch = signBatch * xsimd::pow(xsimd::abs(shapedBatch), expoBatch);
        break;
      case FFEffectSkewMode::Uni:
        auto compBatch = xsimd::lt(shapedBatch, FBBatch<float>(-1.0f));
        compBatch = xsimd::bitwise_or(compBatch, xsimd::gt(shapedBatch, FBBatch<float>(1.0f)));
        auto exceedBatch = FBToBipolar(xsimd::pow(FBToUnipolar(shapedBatch), expoBatch));
        shapedBatch = xsimd::select(compBatch, shapedBatch, exceedBatch);
        break;
      default:
        assert(false);
        break;
      }
      auto mixedBatch = (1.0f - mix) * inBatch + mix * shapedBatch;
      oversampled[c].Store(s, mixedBatch);
    }
  }
}

void 
FFEffectProcessor::ProcessClip(
  int block,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  FBBatch<float> tsqBatch;
  FBBatch<float> signBatch;
  FBBatch<float> exceedBatch1;
  FBBatch<float> exceedBatch2;
  FBBoolBatch<float> compBatch1;
  FBBoolBatch<float> compBatch2;

  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
  {
    auto mix = distMixPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    {
      auto inBatch = oversampled[c].Load(s);
      auto shapedBatch = (inBatch + bias) * drive;
      switch (_clipMode[block])
      {
      case FFEffectClipMode::TanH:
        shapedBatch = xsimd::tanh(shapedBatch);
        break;
      case FFEffectClipMode::Hard:
        shapedBatch = xsimd::clip(shapedBatch, FBBatch<float>(-1.0f), FBBatch<float>(1.0f));
        break;
      case FFEffectClipMode::Inv:
        shapedBatch = xsimd::sign(shapedBatch) * (1.0f - (1.0f / (1.0f + xsimd::abs(30.0f * shapedBatch))));
        break;
      case FFEffectClipMode::Sin:
        signBatch = xsimd::sign(shapedBatch);
        exceedBatch1 = xsimd::sin((shapedBatch * 3.0f * FBPi) / 4.0f);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        shapedBatch = xsimd::select(compBatch1, signBatch, exceedBatch1);
        break;
      case FFEffectClipMode::Cube:
        signBatch = xsimd::sign(shapedBatch);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        exceedBatch1= (9.0f * shapedBatch * 0.25f) - (27.0f * shapedBatch * shapedBatch * shapedBatch / 16.0f);
        shapedBatch = xsimd::select(compBatch1, signBatch, exceedBatch1);
        break;
      case FFEffectClipMode::TSQ:
        signBatch = xsimd::sign(shapedBatch);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        compBatch2 = xsimd::lt(FBBatch<float>(-1.0f / 3.0f), shapedBatch);
        compBatch2 = xsimd::bitwise_and(compBatch2, xsimd::lt(shapedBatch, FBBatch<float>(1.0f / 3.0f)));
        exceedBatch1 = 2.0f * shapedBatch;
        exceedBatch2 = 2.0f - xsimd::abs(3.0f * shapedBatch);
        tsqBatch = signBatch * (3.0f - exceedBatch2 * exceedBatch2) / 3.0f;
        shapedBatch = xsimd::select(compBatch1, signBatch, xsimd::select(compBatch2, exceedBatch1, tsqBatch));
        break;
      case FFEffectClipMode::Exp:
        signBatch = xsimd::sign(shapedBatch);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        exceedBatch1 = signBatch * (1.0f - xsimd::pow(xsimd::abs(1.5f * shapedBatch - signBatch), 0.1f + distAmtPlain[block].Load(s) * 9.9f));
        shapedBatch = xsimd::select(compBatch1, signBatch, exceedBatch1);
        break;
      default:
        assert(false);
        break;
      }
      auto mixedBatch = (1.0f - mix) * inBatch + mix * shapedBatch;
      oversampled[c].Store(s, mixedBatch);
    }
  }
}

void 
FFEffectProcessor::ProcessFold(
  int block,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
  {
    auto mix = distMixPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    { 
      auto inBatch = oversampled[c].Load(s);
      auto shapedBatch = (inBatch + bias) * drive;
      switch (_foldMode[block])
      {
      case FFEffectFoldMode::Fold: shapedBatch = FoldBack(shapedBatch); break;
      case FFEffectFoldMode::Sin: shapedBatch = xsimd::sin(shapedBatch * FBPi); break;
      case FFEffectFoldMode::Cos: shapedBatch = xsimd::cos(shapedBatch * FBPi); break;
      case FFEffectFoldMode::Sin2: shapedBatch = Sin2(shapedBatch); break;
      case FFEffectFoldMode::Cos2: shapedBatch = Cos2(shapedBatch); break;
      case FFEffectFoldMode::SinCos: shapedBatch = SinCos(shapedBatch); break;
      case FFEffectFoldMode::CosSin: shapedBatch = CosSin(shapedBatch); break;
      case FFEffectFoldMode::Sin3: shapedBatch = Sin3(shapedBatch); break;
      case FFEffectFoldMode::Cos3: shapedBatch = Cos3(shapedBatch); break;
      case FFEffectFoldMode::Sn2Cs: shapedBatch = Sn2Cs(shapedBatch); break;
      case FFEffectFoldMode::Cs2Sn: shapedBatch = Cs2Sn(shapedBatch); break;
      case FFEffectFoldMode::SnCs2: shapedBatch = SnCs2(shapedBatch); break;
      case FFEffectFoldMode::CsSn2: shapedBatch = CsSn2(shapedBatch); break;
      case FFEffectFoldMode::SnCsSn: shapedBatch = SnCsSn(shapedBatch); break;
      case FFEffectFoldMode::CsSnCs: shapedBatch = CsSnCs(shapedBatch); break;
      default: assert(false); break;
      }
      auto mixedBatch = (1.0f - mix) * inBatch + mix * shapedBatch;
      oversampled[c].Store(s, mixedBatch);
    }
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

  if (!_on)
  {
    input.CopyTo(output);
    return;
  }

  float sampleRate = state.input->sampleRate;
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
  auto const& trackingKeyNorm = procParams.acc.trackingKey[0].Voice()[voice];

  FBSArray<float, EffectFixedBlockOversamples> trackingKeyPlain;
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
      trackingKeyPlain.Store(s, topo.NormalizedToLinearFast(FFEffectParam::TrackingKey, trackingKeyNorm, s));
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
        combResMinPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombResMin, combResMinNorm[i].Voice()[voice], s));
        combResPlusPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombResPlus, combResPlusNorm[i].Voice()[voice], s));
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
    trackingKeyPlain.UpsampleStretch<EffectOversampleTimes>();
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

  float oversampledRate = _oversampleTimes * sampleRate;
  for(int i = 0; i < FFEffectBlockCount; i++)
    switch (_kind[i])
    {
    case FFEffectKind::Clip:
      ProcessClip(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::Fold:
      ProcessFold(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::Skew:
      ProcessSkew(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::StVar:
      ProcessStVar(i, oversampledRate, oversampled, trackingKeyPlain, stVarResPlain, stVarFreqPlain, stVarGainPlain, stVarKeyTrkPlain);
      break;
    default:
      break;
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
  exchangeParams.acc.trackingKey[0][voice] = trackingKeyNorm.Last();
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