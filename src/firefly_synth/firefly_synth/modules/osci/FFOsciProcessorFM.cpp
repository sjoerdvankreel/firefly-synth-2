#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

static inline float
FMRatioRatio(int v)
{
  FB_ASSERT(0 <= v && v < FFOsciFMRatioCount * FFOsciFMRatioCount);
  return ((v / FFOsciFMRatioCount) + 1.0f) / ((v % FFOsciFMRatioCount) + 1.0f);
}

void 
FFOsciProcessor::BeginVoiceFM(
  FBModuleProcState& state, 
  FBSArray<float, FFOsciUniMaxCount> const& uniPhaseInit)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  auto const& fmModeNorm = params.block.fmMode[0].Voice()[voice];
  auto const& fmRatioModeNorm = params.block.fmRatioMode[0].Voice()[voice];
  auto const& fmRatioRatio12Norm = params.block.fmRatioRatio[0].Voice()[voice];
  auto const& fmRatioRatio23Norm = params.block.fmRatioRatio[1].Voice()[voice];

  _fmMode = topo.NormalizedToListFast<FFOsciFMMode>(FFOsciParam::FMMode, fmModeNorm);
  _fmRatioMode = topo.NormalizedToListFast<FFOsciFMRatioMode>(FFOsciParam::FMRatioMode, fmRatioModeNorm);
  _fmRatioRatio12 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, fmRatioRatio12Norm));
  _fmRatioRatio23 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, fmRatioRatio23Norm));

  _prevUniFMOutput.Fill(0.0f);
  for (int o = 0; o < FFOsciFMOperatorCount; o++)
    for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      _uniFMPhaseGens[o][u / FBSIMDFloatCount] = FFOsciFMPhaseGenerator(uniPhaseInit.Load(u));
}

void 
FFOsciProcessor::ProcessFM(
  FBModuleProcState& state,
  FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
  FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  float sampleRate = state.input->sampleRate;
  float oversampledRate = sampleRate * _oversampleTimes;
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto& voiceState = procState->dsp.voice[voice];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciFMMatrixSize> fmIndexPlain;
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciFMOperatorCount - 1> fmRatioPlain;

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    for (int m = 0; m < FFOsciFMMatrixSize; m++)
    {
      auto const& fmIndexNorm = procParams.acc.fmIndex[m].Voice()[voice];
      fmIndexPlain[m].Store(s, topo.NormalizedToLog2Fast(FFOsciParam::FMIndex, fmIndexNorm, s));
    }
    if (_fmRatioMode == FFOsciFMRatioMode::Ratio)
    {
      fmRatioPlain[0].Store(s, _fmRatioRatio12);
      fmRatioPlain[1].Store(s, _fmRatioRatio23);
    }
    else
    {
      for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
      {
        auto const& fmRatioFreeNorm = procParams.acc.fmRatioFree[o].Voice()[voice];
        fmRatioPlain[o].Store(s, topo.NormalizedToLog2Fast(FFOsciParam::FMRatioFree, fmRatioFreeNorm, s));
      }
    }
  }
  if (_oversampleTimes != 1)
  {
    for (int m = 0; m < FFOsciFMMatrixSize; m++)
      fmIndexPlain[m].UpsampleStretch<FFOsciOversampleTimes>();
    for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
      fmRatioPlain[o].UpsampleStretch<FFOsciOversampleTimes>();
  }

  float applyModMatrixExpoFM = _modMatrixExpoFM ? 1.0f : 0.0f;
  for (int s = 0; s < totalSamples; s++)
  {
    for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
    {
      int block = u / FBSIMDFloatCount;
      auto uniPosMHalfToHalf = _uniPosMHalfToHalf.Load(u);

      FBBatch<float> matrixFMMod(0.0f);
      for (int src = 0; src < state.moduleSlot; src++)
        if (_modSourceFMOn[src] && _modSourceUniCount[src] > u)
        {
          int modSlot = OsciModStartSlot(state.moduleSlot) + src;
          float thatUniOutput = voiceState.osci[src].uniOutputOversampled[u].Get(s);
          matrixFMMod += thatUniOutput * voiceState.osciMod.outputFMIndex[modSlot].Get(s);
        }

      FBBatch<float> op3UniPitch = basePitchPlain.Get(s) + uniPosMHalfToHalf * uniDetunePlain.Get(s);
      op3UniPitch += matrixFMMod * op3UniPitch * applyModMatrixExpoFM;
      auto op3UniFreq = FBPitchToFreq(op3UniPitch);
      
      FBBatch<float> op1UniPhase;
      FBBatch<float> fmTo1 = 0.0f;
      fmTo1 += fmIndexPlain[0].Get(s) * _prevUniFMOutput[0].Load(u);
      fmTo1 += fmIndexPlain[3].Get(s) * _prevUniFMOutput[1].Load(u);
      fmTo1 += fmIndexPlain[6].Get(s) * _prevUniFMOutput[2].Load(u);
      if (_fmMode == FFOsciFMMode::Exp)
      {
        auto op1UniPitch = op3UniPitch / fmRatioPlain[1].Get(s) / fmRatioPlain[0].Get(s);
        op1UniPitch += op1UniPitch * fmTo1;
        auto op1UniFreq = FBPitchToFreq(op1UniPitch);
        op1UniPhase = _uniFMPhaseGens[0][block].Next(op1UniFreq / oversampledRate, 0.0f);
      }
      else
      {
        auto op1UniFreq = op3UniFreq / fmRatioPlain[1].Get(s) / fmRatioPlain[0].Get(s);
        op1UniPhase = _uniFMPhaseGens[0][block].Next(op1UniFreq / oversampledRate, fmTo1);
      }
      auto output1 = xsimd::sin(op1UniPhase * FBTwoPi);

      FBBatch<float> op2UniPhase;
      FBBatch<float> fmTo2 = 0.0f;
      fmTo2 += fmIndexPlain[1].Get(s) * output1;
      fmTo2 += fmIndexPlain[4].Get(s) * _prevUniFMOutput[1].Load(u);
      fmTo2 += fmIndexPlain[7].Get(s) * _prevUniFMOutput[2].Load(u);
      if (_fmMode == FFOsciFMMode::Exp)
      {
        auto op2UniPitch = op3UniPitch / fmRatioPlain[1].Get(s);
        op2UniPitch += op2UniPitch * fmTo2;
        auto op2UniFreq = FBPitchToFreq(op2UniPitch);
        op2UniPhase = _uniFMPhaseGens[1][block].Next(op2UniFreq / oversampledRate, 0.0f);
      }
      else
      {
        auto op2UniFreq = op3UniFreq / fmRatioPlain[1].Get(s);
        op2UniPhase = _uniFMPhaseGens[1][block].Next(op2UniFreq / oversampledRate, fmTo2);
      }
      auto output2 = xsimd::sin(op2UniPhase * FBTwoPi);

      FBBatch<float> op3UniPhase;
      FBBatch<float> fmTo3 = 0.0f;
      fmTo3 += fmIndexPlain[2].Get(s) * output1;
      fmTo3 += fmIndexPlain[5].Get(s) * output2;
      fmTo3 += fmIndexPlain[8].Get(s) * _prevUniFMOutput[2].Load(u);
      if (_fmMode == FFOsciFMMode::Exp)
      {
        op3UniPitch += op3UniPitch * fmTo3;
        op3UniFreq = FBPitchToFreq(op3UniPitch);
        op3UniPhase = _uniFMPhaseGens[2][block].Next(op3UniFreq / oversampledRate, 0.0f);
      }
      else
      {
        op3UniPhase = _uniFMPhaseGens[2][block].Next(op3UniFreq / oversampledRate, fmTo3 + matrixFMMod);
      }
      auto output3 = xsimd::sin(op3UniPhase * FBTwoPi);

      _prevUniFMOutput[0].Store(u, output1);
      _prevUniFMOutput[1].Store(u, output2);
      _prevUniFMOutput[2].Store(u, output3);

      FBSArray<float, FBSIMDFloatCount> outputArray;
      outputArray.Store(0, output3);
      for (int v = 0; v < FBSIMDFloatCount; v++)
        uniOutputOversampled[u + v].Set(s, outputArray.Get(v));
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
  {
    auto const& fmRatioFreeNorm = procParams.acc.fmRatioFree[o].Voice()[voice];
    exchangeParams.acc.fmRatioFree[o][voice] = fmRatioFreeNorm.Last();
  }
  for (int m = 0; m < FFOsciFMMatrixSize; m++)
  {
    auto const& fmIndexNorm = procParams.acc.fmIndex[m].Voice()[voice];
    exchangeParams.acc.fmIndex[m][voice] = fmIndexNorm.Last();
  }
}
