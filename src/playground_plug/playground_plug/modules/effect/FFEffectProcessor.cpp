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

static float const InvLogHalf = 1.0f / std::log(0.5f);

template <class T>
static inline T
Sin2(T in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  return xsimd::sin(in * FBPi + sin1);
}

template <class T>
static inline T
Cos2(T in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  return xsimd::cos(in * FBPi + cos1);
}

template <class T>
static inline T
SinCos(T in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  return xsimd::sin(in * FBPi + cos1);
}

template <class T>
static inline T
CosSin(T in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  return xsimd::cos(in * FBPi + sin1);
}

template <class T>
static inline T
Sin3(T in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + sin1);
  return xsimd::sin(in * FBPi + sin2);
}

template <class T>
static inline T
Cos3(T in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + cos1);
  return xsimd::cos(in * FBPi + cos2);
}

template <class T>
static inline T
Sn2Cs(T in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + cos1);
  return xsimd::sin(in * FBPi + sin2);
}

template <class T>
static inline T
Cs2Sn(T in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + sin1);
  return xsimd::cos(in * FBPi + cos2);
}

template <class T>
static inline T
SnCs2(T in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + cos1);
  return xsimd::sin(in * FBPi + cos2);
}

template <class T>
static inline T
CsSn2(T in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + sin1);
  return xsimd::cos(in * FBPi + sin2);
}

template <class T>
static inline T
SnCsSn(T in)
{
  auto sin1 = xsimd::sin(in * FBPi);
  auto cos2 = xsimd::cos(in * FBPi + sin1);
  return xsimd::sin(in * FBPi + cos2);
}

template <class T>
static inline T
CsSnCs(T in)
{
  auto cos1 = xsimd::cos(in * FBPi);
  auto sin2 = xsimd::sin(in * FBPi + cos1);
  return xsimd::cos(in * FBPi + sin2);
}

static inline float
FoldBack(float x)
{
  x = xsimd::clip(x, -32.0f, 32.0f);
  while (true)
  {
    if (x > 1.0f) x -= 2.0f * (x - 1.0f);
    else if (x < -1.0f) x += 2.0f * (-x - 1.0f);
    else break;
  }
  return x;
}

static inline FBBatch<float>
FoldBack(FBBatch<float> x)
{
  FBSArray<float, FBSIMDFloatCount> y;
  y.Store(0, x);
  for (int i = 0; i < FBSIMDFloatCount; i++)
    y.Set(i, FoldBack(y.Get(i)));
  return y.Load(0);
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

  _key = static_cast<float>(state.voice->event.note.key);
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
    _stVarFilters[i] = {};
  }
}  

void
FFEffectProcessor::ProcessStVarSample(
  int block, float sampleRate, int sample,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray<float, EffectFixedBlockOversamples> const& trackingKeyPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarResPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarFreqPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarGainPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarKeyTrkPlain)
{
  auto trkk = trackingKeyPlain.Get(sample);
  auto res = stVarResPlain[block].Get(sample);
  auto freq = stVarFreqPlain[block].Get(sample);
  auto gain = stVarGainPlain[block].Get(sample);
  auto ktrk = stVarKeyTrkPlain[block].Get(sample);
  freq *= std::pow(2.0f, (_key - 60.0f + trkk) / 12.0f * ktrk);
  freq = std::clamp(freq, 20.0f, 20000.0f);
  _stVarFilters[block].Set(_stVarMode[block], sampleRate, freq, res, gain);
  for (int c = 0; c < 2; c++)
    oversampled[c].Set(sample, _stVarFilters[block].Next(c, oversampled[c].Get(sample)));
}

void 
FFEffectProcessor::ProcessStVarBuffer(
  int block, float sampleRate,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray<float, EffectFixedBlockOversamples> const& trackingKeyPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarResPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarFreqPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarGainPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarKeyTrkPlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s++)
    ProcessStVarSample(
      block, sampleRate, s, 
      oversampled, trackingKeyPlain, 
      stVarResPlain, stVarFreqPlain, stVarGainPlain, stVarKeyTrkPlain);
}

template <class T>
inline T 
FFEffectProcessor::ProcessSkewSampleOrBatch(
  int block, T in, T amt)
{
  T exceed; 
  decltype(xsimd::lt(T(), T())) comp;
  T sign = xsimd::sign(in);
  T expo = xsimd::log(0.01f + amt * 0.98f) * InvLogHalf;
  switch (_skewMode[block])
  {
  case FFEffectSkewMode::Bi:
    return sign * xsimd::pow(xsimd::abs(in), expo);
  case FFEffectSkewMode::Uni:
    comp = xsimd::lt(in, T(-1.0f));
    comp = xsimd::bitwise_or(comp, xsimd::gt(in, T(1.0f)));
    exceed = FBToBipolar(xsimd::pow(FBToUnipolar(in), expo));
    return xsimd::select(comp, in, exceed);
  default:
    assert(false);
    return {};
  }
}

void 
FFEffectProcessor::ProcessSkewBuffer(
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
    auto amt = distAmtPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    {
      auto in = oversampled[c].Load(s);
      auto shaped = (in + bias) * drive;
      shaped = ProcessSkewSampleOrBatch(block, shaped, amt);
      auto mixed = (1.0f - mix) * in + mix * shaped;
      oversampled[c].Store(s, mixed);
    }
  }
}

void
FFEffectProcessor::ProcessSkewSample(
  int block,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s ++)
  {
    auto mix = distMixPlain[block].Get(s);
    auto amt = distAmtPlain[block].Get(s);
    auto bias = distBiasPlain[block].Get(s);
    auto drive = distDrivePlain[block].Get(s);
    for (int c = 0; c < 2; c++)
    {
      auto in = oversampled[c].Get(s);
      auto shaped = (in + bias) * drive;
      shaped = ProcessSkewSampleOrBatch(block, shaped, amt);
      auto mixed = (1.0f - mix) * in + mix * shaped;
      oversampled[c].Set(s, mixed);
    }
  }
}

template <class T>
inline T 
FFEffectProcessor::ProcessClipSampleOrBatch(
  int block, T in, T amt)
{
  T tsq;
  T sign;
  T exceed1;
  T exceed2;
  decltype(xsimd::lt(T(), T())) comp1;
  decltype(xsimd::lt(T(), T())) comp2;
  switch (_clipMode[block])
  {
  case FFEffectClipMode::TanH:
    return xsimd::tanh(in);
  case FFEffectClipMode::Hard:
    return xsimd::clip(in, T(-1.0f), T(1.0f));
  case FFEffectClipMode::Inv:
    return xsimd::sign(in) * (1.0f - (1.0f / (1.0f + xsimd::abs(30.0f * in))));
    break;
  case FFEffectClipMode::Sin:
    sign = xsimd::sign(in);
    exceed1 = xsimd::sin((in * 3.0f * FBPi) / 4.0f);
    comp1 = xsimd::gt(xsimd::abs(in), T(2.0f / 3.0f));
    return xsimd::select(comp1, sign, exceed1);
  case FFEffectClipMode::Cube:
    sign = xsimd::sign(in);
    comp1 = xsimd::gt(xsimd::abs(in), T(2.0f / 3.0f));
    exceed1 = (9.0f * in * 0.25f) - (27.0f * in * in * in / 16.0f);
    return xsimd::select(comp1, sign, exceed1);
    break;
  case FFEffectClipMode::TSQ:
    sign = xsimd::sign(in);
    comp1 = xsimd::gt(xsimd::abs(in), T(2.0f / 3.0f));
    comp2 = xsimd::lt(T(-1.0f / 3.0f), in);
    comp2 = xsimd::bitwise_and(comp2, xsimd::lt(in, T(1.0f / 3.0f)));
    exceed1 = 2.0f * in;
    exceed2 = 2.0f - xsimd::abs(3.0f * in);
    tsq = sign * (3.0f - exceed2 * exceed2) / 3.0f;
    return xsimd::select(comp1, sign, xsimd::select(comp2, exceed1, tsq));
    break;
  case FFEffectClipMode::Exp:
    sign = xsimd::sign(in);
    comp1 = xsimd::gt(xsimd::abs(in), T(2.0f / 3.0f));
    exceed1 = sign * (1.0f - xsimd::pow(xsimd::abs(1.5f * in - sign), 0.1f + amt * 9.9f));
    return xsimd::select(comp1, sign, exceed1);
    break;
  default:
    assert(false);
    return {};
  }
}

void 
FFEffectProcessor::ProcessClipBuffer(
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
    auto amt = distAmtPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    {
      auto in = oversampled[c].Load(s);
      auto shaped = (in + bias) * drive;
      shaped = ProcessClipSampleOrBatch(block, shaped, amt);
      auto mixed = (1.0f - mix) * in + mix * shaped;
      oversampled[c].Store(s, mixed);
    }
  }
}

void
FFEffectProcessor::ProcessClipSample(
  int block,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s++)
  {
    auto mix = distMixPlain[block].Get(s);
    auto amt = distAmtPlain[block].Get(s);
    auto bias = distBiasPlain[block].Get(s);
    auto drive = distDrivePlain[block].Get(s);
    for (int c = 0; c < 2; c++)
    {
      auto in = oversampled[c].Get(s);
      auto shaped = (in + bias) * drive;
      shaped = ProcessClipSampleOrBatch(block, shaped, amt);
      auto mixed = (1.0f - mix) * in + mix * shaped;
      oversampled[c].Set(s, mixed);
    }
  }
}

template <class T>
inline T 
FFEffectProcessor::ProcessFoldSampleOrBatch(
  int block, T in)
{
  switch (_foldMode[block])
  {
  case FFEffectFoldMode::Fold: return FoldBack(in); break;
  case FFEffectFoldMode::Sin: return xsimd::sin(in * FBPi); break;
  case FFEffectFoldMode::Cos: return xsimd::cos(in * FBPi); break;
  case FFEffectFoldMode::Sin2: return Sin2(in); break;
  case FFEffectFoldMode::Cos2: return Cos2(in); break;
  case FFEffectFoldMode::SinCos: return SinCos(in); break;
  case FFEffectFoldMode::CosSin: return CosSin(in); break;
  case FFEffectFoldMode::Sin3: return Sin3(in); break;
  case FFEffectFoldMode::Cos3: return Cos3(in); break;
  case FFEffectFoldMode::Sn2Cs: return Sn2Cs(in); break;
  case FFEffectFoldMode::Cs2Sn: return Cs2Sn(in); break;
  case FFEffectFoldMode::SnCs2: return SnCs2(in); break;
  case FFEffectFoldMode::CsSn2: return CsSn2(in); break;
  case FFEffectFoldMode::SnCsSn: return SnCsSn(in); break;
  case FFEffectFoldMode::CsSnCs: return CsSnCs(in); break;
  default: assert(false); return {};
  }
}

void 
FFEffectProcessor::ProcessFoldBuffer(
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
      auto in = oversampled[c].Load(s);
      auto shaped = (in + bias) * drive;
      shaped = ProcessFoldSampleOrBatch(block, shaped);
      auto mixed = (1.0f - mix) * in + mix * shaped;
      oversampled[c].Store(s, mixed);
    }
  }
}

void
FFEffectProcessor::ProcessFoldSample(
  int block,
  FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s ++)
  {
    auto mix = distMixPlain[block].Get(s);
    auto bias = distBiasPlain[block].Get(s);
    auto drive = distDrivePlain[block].Get(s);
    for (int c = 0; c < 2; c++)
    {
      auto in = oversampled[c].Get(s);
      auto shaped = (in + bias) * drive;
      shaped = ProcessFoldSampleOrBatch(block, shaped);
      auto mixed = (1.0f - mix) * in + mix * shaped;
      oversampled[c].Set(s, mixed);
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

  if (_type == FFEffectType::Off)
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
  auto const& feedbackNorm = procParams.acc.feedback[0].Voice()[voice];
  auto const& trackingKeyNorm = procParams.acc.trackingKey[0].Voice()[voice];

  FBSArray<float, EffectFixedBlockOversamples> feedbackPlain;
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
      feedbackPlain.Store(s, topo.NormalizedToIdentityFast(FFEffectParam::Feedback, feedbackNorm, s));
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
    feedbackPlain.UpsampleStretch<EffectOversampleTimes>();
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

  if(_type == FFEffectType::On)
    for(int i = 0; i < FFEffectBlockCount; i++)
      switch (_kind[i])
      {
      case FFEffectKind::Clip:
        ProcessClipBuffer(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
        break;
      case FFEffectKind::Fold:
        ProcessFoldBuffer(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
        break;
      case FFEffectKind::Skew:
        ProcessSkewBuffer(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
        break;
      case FFEffectKind::StVar:
        ProcessStVarBuffer(i, sampleRate, oversampled, trackingKeyPlain, stVarResPlain, stVarFreqPlain, stVarGainPlain, stVarKeyTrkPlain);
        break;
      default:
        break;
      }
  else
    for (int i = 0; i < FFEffectBlockCount; i++)
      switch (_kind[i])
      {
      case FFEffectKind::Clip:
        ProcessClipSample(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
        break;
      case FFEffectKind::Fold:
        ProcessFoldSample(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
        break;
      case FFEffectKind::Skew:
        ProcessSkewSample(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
        break;
      case FFEffectKind::StVar:
        ProcessStVarBuffer(i, sampleRate, oversampled, trackingKeyPlain, stVarResPlain, stVarFreqPlain, stVarGainPlain, stVarKeyTrkPlain);
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
  exchangeParams.acc.feedback[0][voice] = feedbackNorm.Last();
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