#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

static FBFloatVector
GenerateSin(FBFloatVector phase)
{
  return xsimd::sin(phase * FBTwoPi);
}

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
static FBFloatVector
GenerateSaw(FBFloatVector phase, FBFloatVector incr)
{ 
  FBFloatVector one = 1.0f;
  FBFloatVector zero = 0.0f;
  FBFloatVector blepLo = phase / incr;
  FBFloatVector blepHi = (phase - 1.0f) / incr;
  FBFloatVector result = phase * 2.0f - 1.0f;
  auto loMask = xsimd::lt(phase, incr);
  auto hiMask = xsimd::ge(phase, 1.0f - incr);
  hiMask = xsimd::bitwise_andnot(hiMask, loMask);
  FBFloatVector loMul = xsimd::select(loMask, one, zero);
  FBFloatVector hiMul = xsimd::select(hiMask, one, zero);
  result -= loMul * ((2.0f - blepLo) * blepLo - 1.0f);
  result -= hiMul * ((blepHi + 2.0f) * blepHi + 1.0f);
  return result;
}

static FBFloatVector
GenerateSqr(FBFloatVector phase, FBFloatVector incr, FBFloatVector pw)
{
  FBFloatVector minPW = 0.05f;
  FBFloatVector realPW = (minPW + (1.0f - minPW) * pw) * 0.5f;
  FBFloatVector phase2 = phase + realPW;
  phase2 -= xsimd::floor(phase2);
  return (GenerateSaw(phase, incr) - GenerateSaw(phase2, incr)) * 0.5f;
}

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
static FBFloatVector 
GenerateBLAMP(FBFloatVector phase, FBFloatVector incr)
{
  FBFloatVector y = 0.0f;
  FBFloatVector one = 1.0f;
  FBFloatVector zero = 0.0f;
  auto phaseLtIncrMask = xsimd::lt(phase, incr);
  auto phaseGte0Mask = xsimd::ge(phase, FBFloatVector(0.0f));
  auto phaseLt2IncrMask = xsimd::lt(phase, FBFloatVector(2.0f) * incr);
  auto phaseBetween0And2IncrMask = xsimd::bitwise_and(phaseGte0Mask, phaseLt2IncrMask);
  FBFloatVector phaseLtIncrMul = xsimd::select(phaseLtIncrMask, one, zero);
  FBFloatVector phaseBetween0And2IncrMul = xsimd::select(phaseBetween0And2IncrMask, one, zero);
  FBFloatVector x = phase / incr;
  FBFloatVector u = 2.0f - x;
  FBFloatVector u2 = u * u;
  u *= u2 * u2;
  y -= phaseBetween0And2IncrMul * u;
  FBFloatVector v = 1.0f - x;
  FBFloatVector v2 = v * v;
  v *= v2 * v2;
  y += 4.0f * phaseBetween0And2IncrMul * phaseLtIncrMul * v;
  return y * incr / 15.0f;
}

static FBFloatVector
GenerateTri(FBFloatVector phase, FBFloatVector incr)
{
  FBFloatVector v = 2.0f * xsimd::abs(2.0f * phase - 1.0f) - 1.0f;
  v += GenerateBLAMP(phase, incr);
  v += GenerateBLAMP(1.0f - phase, incr);
  phase += 0.5f;
  phase -= xsimd::floor(phase);
  v -= GenerateBLAMP(phase, incr);
  v -= GenerateBLAMP(1.0f - phase, incr);
  return v;
}

void 
FFOsciProcessor::BeginVoice(FBModuleProcState const& state)
{
  _phase = {};
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  _voiceState.key = static_cast<float>(state.voice->event.note.key);
  _voiceState.note = topo.params[(int)FFOsciParam::Note].Note().NormalizedToPlainFast(params.block.note[0].Voice()[voice]);
  _voiceState.type = (FFOsciType)topo.params[(int)FFOsciParam::Type].List().NormalizedToPlainFast(params.block.type[0].Voice()[voice]);
  _voiceState.type = (FFOsciType)topo.params[(int)FFOsciParam::Type].List().NormalizedToPlainFast(params.block.type[0].Voice()[voice]);
  _voiceState.basicSinOn = topo.params[(int)FFOsciParam::BasicSinOn].Boolean().NormalizedToPlainFast(params.block.basicSinOn[0].Voice()[voice]);
  _voiceState.basicSawOn = topo.params[(int)FFOsciParam::BasicSawOn].Boolean().NormalizedToPlainFast(params.block.basicSawOn[0].Voice()[voice]);
  _voiceState.basicTriOn = topo.params[(int)FFOsciParam::BasicTriOn].Boolean().NormalizedToPlainFast(params.block.basicTriOn[0].Voice()[voice]);
  _voiceState.basicSqrOn = topo.params[(int)FFOsciParam::BasicSqrOn].Boolean().NormalizedToPlainFast(params.block.basicSqrOn[0].Voice()[voice]);
  _voiceState.dsfDistance = topo.params[(int)FFOsciParam::DSFDistance].Linear().NormalizedToPlainFast(params.block.dsfDistance[0].Voice()[voice]);
  _voiceState.dsfOvertones = topo.params[(int)FFOsciParam::DSFOvertones].DiscreteLog2().NormalizedToPlainFast(params.block.dsfOvertones[0].Voice()[voice]);
}

void 
FFOsciProcessor::ProcessBasic(
  FBModuleProcState& state, 
  FBFixedFloatBlock const& phase,
  FBFixedFloatBlock const& incr,
  FBFixedFloatBlock& audioOut)
{
  FBFixedFloatBlock pw;
  FBFixedFloatBlock gain;
  FBFixedFloatBlock audio;

  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& basicSqrPW = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& basicSinGain = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGain = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGain = procParams.acc.basicTriGain[0].Voice()[voice];
  auto const& basicSqrGain = procParams.acc.basicSqrGain[0].Voice()[voice];

  audioOut.Fill(0.0f);
  if (_voiceState.basicSinOn)
  {
    gain.Transform([&](int v) {
      auto const& paramTopo = topo.params[(int)FFOsciParam::BasicSinGain];
      return paramTopo.Linear().NormalizedToPlainFast(basicSinGain.CV(v)); });
    audio.Transform([&](int v) { return GenerateSin(phase[v]) * gain[v]; });
    audioOut.Add(audio);
  }
  if (_voiceState.basicSawOn)
  {
    gain.Transform([&](int v) {
      auto const& paramTopo = topo.params[(int)FFOsciParam::BasicSawGain];
      return paramTopo.Linear().NormalizedToPlainFast(basicSawGain.CV(v)); });
    audio.Transform([&](int v) { return GenerateSaw(phase[v], incr[v]) * gain[v]; });
    audioOut.Add(audio);
  }
  if (_voiceState.basicTriOn)
  {
    gain.Transform([&](int v) {
      auto const& paramTopo = topo.params[(int)FFOsciParam::BasicTriGain];
      return paramTopo.Linear().NormalizedToPlainFast(basicTriGain.CV(v)); });
    audio.Transform([&](int v) { return GenerateTri(phase[v], incr[v]) * gain[v]; });
    audioOut.Add(audio);
  }
  if (_voiceState.basicSqrOn)
  {
    pw.Transform([&](int v) {
      auto const& paramTopo = topo.params[(int)FFOsciParam::BasicSqrPW];
      return topo.params[(int)FFOsciParam::BasicSqrPW].Linear().NormalizedToPlainFast(basicSqrPW.CV(v)); });
    gain.Transform([&](int v) {
      auto const& paramTopo = topo.params[(int)FFOsciParam::BasicSqrGain];
      return topo.params[(int)FFOsciParam::BasicSqrGain].Linear().NormalizedToPlainFast(basicSqrGain.CV(v)); });
    audio.Transform([&](int v) { return GenerateSqr(phase[v], incr[v], pw[v]) * gain[v]; });
    audioOut.Add(audio);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.basicSqrPW[0][voice] = basicSqrPW.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicSinGain[0][voice] = basicSinGain.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicSawGain[0][voice] = basicSawGain.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicTriGain[0][voice] = basicTriGain.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicSqrGain[0][voice] = basicSqrGain.CV().data[FBFixedBlockSamples - 1];
}

int
FFOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& cent = procParams.acc.cent[0].Voice()[voice];
  auto const& gain = procParams.acc.gain[0].Voice()[voice];
  auto const& gLFOToGain = procParams.acc.gLFOToGain[0].Voice()[voice];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& output = procState->dsp.voice[voice].osci[state.moduleSlot].output;

  if (_voiceState.type == FFOsciType::Off)
  {
    output.Fill(0.0f);
    return 0;
  }

  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();

  FBFixedFloatBlock incr;
  FBFixedFloatBlock freq;
  FBFixedFloatBlock phase;
  freq.Transform([&](int v) {
    auto centPlain = topo.params[(int)FFOsciParam::Cent].Linear().NormalizedToPlainFast(cent.CV(v));
    auto pitch = _voiceState.key + _voiceState.note - 60.0f + centPlain;
    return FBPitchToFreq(pitch, state.input->sampleRate); });
  incr.Transform([&](int v) { return freq[v] / state.input->sampleRate; });
  phase.Transform([&](int v) { return _phase.Next(incr[v]); });

  FBFixedFloatBlock osciOut;
  if (_voiceState.type == FFOsciType::Basic)
    ProcessBasic(state, phase, incr, osciOut);

  // TODO this might prove difficult, lets see how it fares with the matrices
  FBFixedFloatBlock gLFO;
  auto* exchangeFromGUI = state.ExchangeFromGUIAs<FFExchangeState>();
  if (exchangeFromGUI != nullptr)
    gLFO.Fill(exchangeFromGUI->global.gLFO[0].lastOutput);
  else
    gLFO.CopyFrom(procState->dsp.global.gLFO[0].output);

  FBFixedFloatBlock gainWithGLFOBlock;
  gainWithGLFOBlock.Transform([&](int v) {
    auto gLFOToGainBlock = gLFOToGain.CV(v);
    return (1.0f - gLFOToGainBlock) * gain.CV(v) + gLFOToGainBlock * gLFO[v] * gain.CV(v); });

  osciOut.Transform([&](int v) {
    return gainWithGLFOBlock[v] * osciOut[v]; });
  output.Transform([&](int ch, int v) { return osciOut[v]; });

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(freq.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.gain[0][voice] = gainWithGLFOBlock.Last();
  exchangeParams.acc.cent[0][voice] = cent.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGain.CV().data[FBFixedBlockSamples - 1];
  return FBFixedBlockSamples;
}