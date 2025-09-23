#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/plug/FFVoiceProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/note/FFVNoteProcessor.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

FFVEchoTarget 
FFVoiceProcessor::GetCurrentVEchoTarget(FBModuleProcState const& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& vEcho = procState->param.voice.vEcho[0];
  auto& vEchoModuleTopo = state.topo->static_->modules[(int)FFModuleType::VEcho];
  float vEchoTargetNorm = vEcho.block.vTargetOrGTarget[0].Voice()[voice];
  return vEchoModuleTopo.NormalizedToListFast<FFVEchoTarget>(FFEchoParam::VTargetOrGTarget, vEchoTargetNorm);
}

void 
FFVoiceProcessor::BeginVoice(
  FBModuleProcState state,
  std::array<float, FFVNoteOnNoteRandomCount> const& onNoteRandomUni,
  std::array<float, FFVNoteOnNoteRandomCount> const& onNoteRandomNorm,
  std::array<float, FFVNoteOnNoteRandomCount> const& onNoteGroupRandomUni,
  std::array<float, FFVNoteOnNoteRandomCount> const& onNoteGroupRandomNorm)
  {
  // We need to handle modules BeginVoice inside process
  // because of the on-voice-start modulation feature.
  // F.e. lfo output can be targeted to env stage length
  // which is handle in env's beginvoice, not process.
  _firstRoundThisVoice = true;
  _onNoteRandomUni = onNoteRandomUni;
  _onNoteRandomNorm = onNoteRandomNorm;
  _onNoteGroupRandomUni = onNoteGroupRandomUni;
  _onNoteGroupRandomNorm = onNoteGroupRandomNorm;

  // This one actually needs to be here.
  state.moduleSlot = 0;
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  procState->dsp.voice[voice].vMatrix.processor->BeginVoiceOrBlock(state);
}

bool 
FFVoiceProcessor::Process(FBModuleProcState state, int releaseAt)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceDSP = procState->dsp.voice[voice];
  auto const& vMix = procState->param.voice.vMix[0];
  auto const& balNormIn = vMix.acc.bal[0].Voice()[voice];
  auto const& ampNormIn = vMix.acc.amp[0].Voice()[voice];
  auto const& lfo6ToBalNorm = vMix.acc.lfo6ToBal[0].Voice()[voice];
  auto const& ampEnvToAmpNorm = vMix.acc.ampEnvToAmp[0].Voice()[voice];
  auto const& osciMixToOutNorm = vMix.acc.osciMixToOut[0].Voice()[voice];
  auto& moduleTopo = state.topo->static_->modules[(int)FFModuleType::VMix];

  FBSArray2<float, FBFixedBlockSamples, 2> osciMix = {};
  FBSArray<float, FBFixedBlockSamples> ampNormModulated = {};
  FBSArray<float, FBFixedBlockSamples> balNormModulated = {};
  osciMix.Fill(0.0f);

  state.moduleSlot = 0;
  procState->dsp.voice[voice].vMatrix.processor->BeginModulationBlock();

  // No need to process VNote, values are fixed at BeginVoice.
  state.moduleSlot = 0;
  if (_firstRoundThisVoice)
    procState->dsp.voice[voice].vNote.processor->BeginVoice(state, 
      _onNoteRandomUni, _onNoteRandomNorm, _onNoteGroupRandomUni, _onNoteGroupRandomNorm);
  procState->dsp.voice[voice].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::VNote, 0 });

  // This needs to be before init of the voice-amp, because of the porta section attack.
  if (_firstRoundThisVoice)
  {
    bool anyNoteWasOnAlready = false;
    float previousMidiKeyUntuned = -1.0f;
    int voiceStartSamplesInBlock = state.voice->offsetInBlock;
    if (voiceStartSamplesInBlock == 0)
    {
      anyNoteWasOnAlready = state.input->anyNoteWasOnLastSamplePrevRound;
      previousMidiKeyUntuned = state.input->lastKeyRawLastSamplePrevRound * 127.0f;
    }
    else
    {
      anyNoteWasOnAlready = (*state.input->anyNoteIsOn)[voiceStartSamplesInBlock - 1];
      previousMidiKeyUntuned = state.input->noteMatrixRaw->entries[(int)FBNoteMatrixEntry::LastKeyUntuned].Get(voiceStartSamplesInBlock - 1) * 127.0f;
    }

    state.moduleSlot = 0;
    procState->dsp.voice[voice].voiceModule.processor->BeginVoice(state, previousMidiKeyUntuned, anyNoteWasOnAlready);
  }

  state.moduleSlot = FFAmpEnvSlot;
  if (_firstRoundThisVoice)
    voiceDSP.env[FFAmpEnvSlot].processor->BeginVoice(state, nullptr, false);
  int ampEnvProcessed = voiceDSP.env[FFAmpEnvSlot].processor->Process(state, nullptr, false, releaseAt);
  bool voiceFinished = ampEnvProcessed != FBFixedBlockSamples;
  state.moduleSlot = 0;
  procState->dsp.voice[voice].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::Env, FFAmpEnvSlot });

  for (int i = 0; i < FFLFOCount; i++)
  {
    state.moduleSlot = i + FFEnvSlotOffset;
    if(_firstRoundThisVoice)
      voiceDSP.env[i + FFEnvSlotOffset].processor->BeginVoice(state, nullptr, false);
    voiceDSP.env[i + FFEnvSlotOffset].processor->Process(state, nullptr, false, releaseAt);
    state.moduleSlot = 0;
    procState->dsp.voice[voice].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::Env, i + FFEnvSlotOffset });

    state.moduleSlot = i;
    if(_firstRoundThisVoice)
      voiceDSP.vLFO[i].processor->BeginVoiceOrBlock<false>(state, nullptr, false, -1, -1);
    voiceDSP.vLFO[i].processor->Process<false>(state);
    state.moduleSlot = 0;
    procState->dsp.voice[voice].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::VLFO, i });
  }

  state.moduleSlot = 0;
  voiceDSP.voiceModule.processor->Process(state);
  
  state.moduleSlot = 0;
  if(_firstRoundThisVoice)
    voiceDSP.osciMod.processor->BeginVoice(state, false);
  voiceDSP.osciMod.processor->Process(state);

  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    if(_firstRoundThisVoice)
      voiceDSP.osci[i].processor->BeginVoice(state, false);
    voiceDSP.osci[i].processor->Process(state, false);
    osciMix.AddMul(voiceDSP.osci[i].output, vMix.acc.osciToOsciMix[i].Voice()[voice].CV());
  }

  FB_ASSERT(FFOsciCount == FFEffectCount);
  for (int i = 0; i < FFEffectCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.vEffect[i].input.Fill(0.0f);
    for (int r = 0; r < FFEffectCount; r++)
    {
      auto const& osciMixToVFXNorm = vMix.acc.osciMixToVFX[r].Voice()[voice].CV();
      voiceDSP.vEffect[r].input.AddMul(osciMix, osciMixToVFXNorm);
    }
    for (int r = 0; r < FFVMixOsciToVFXCount; r++)
      if (FFVMixOsciToVFXGetFXSlot(r) == i)
      {
        int o = FFVMixOsciToVFXGetOsciSlot(r);
        auto const& osciToVFXNorm = vMix.acc.osciToVFX[r].Voice()[voice].CV();
        voiceDSP.vEffect[i].input.AddMul(voiceDSP.osci[o].output, osciToVFXNorm);
      }
    for (int r = 0; r < FFMixFXToFXCount; r++)
      if (FFMixFXToFXGetTargetSlot(r) == i)
      {
        int source = FFMixFXToFXGetSourceSlot(r);
        auto const& vfxToVFXNorm = vMix.acc.VFXToVFX[r].Voice()[voice].CV();
        voiceDSP.vEffect[i].input.AddMul(voiceDSP.vEffect[source].output, vfxToVFXNorm);
      }

    state.moduleSlot = i;
    if (_firstRoundThisVoice)
      voiceDSP.vEffect[i].processor->BeginVoiceOrBlock<false>(state, false, -1, -1);
    voiceDSP.vEffect[i].processor->Process<false>(state);
  }

  voiceDSP.output.Fill(0.0f);
  voiceDSP.output.AddMul(osciMix, osciMixToOutNorm.CV());
  for (int i = 0; i < FFOsciCount; i++)
  {
    auto const& osciToOutNorm = vMix.acc.osciToOut[i].Voice()[voice].CV();
    voiceDSP.output.AddMul(voiceDSP.osci[i].output, osciToOutNorm);
  }
  for (int i = 0; i < FFEffectCount; i++)
  {
    auto const& vfxToOutNorm = vMix.acc.VFXToOut[i].Voice()[voice].CV();
    voiceDSP.output.AddMul(voiceDSP.vEffect[i].output, vfxToOutNorm);
  }

  balNormIn.CV().CopyTo(balNormModulated);
  ampNormIn.CV().CopyTo(ampNormModulated);
  FFApplyModulation(FFModulationOpType::BPStack, voiceDSP.vLFO[5].outputAll, lfo6ToBalNorm.CV(), balNormModulated);
  FFApplyModulation(FFModulationOpType::UPMul, voiceDSP.env[FFAmpEnvSlot].output, ampEnvToAmpNorm.CV(), ampNormModulated);
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float balPlain = moduleTopo.NormalizedToLinearFast(FFVMixParam::Bal, balNormModulated.Get(s));
    float ampPlain = moduleTopo.NormalizedToLinearFast(FFVMixParam::Amp, ampNormModulated.Get(s));
    for (int c = 0; c < 2; c++)
      voiceDSP.output[c].Set(s, voiceDSP.output[c].Get(s) * ampPlain * FBStereoBalance(c, balPlain));
  }

  if (GetCurrentVEchoTarget(state) == FFVEchoTarget::MixOut)
  {
    state.moduleSlot = 0;
    if(_firstRoundThisVoice)
      voiceDSP.vEcho.processor->BeginVoiceOrBlock(state, false, -1, -1);
    voiceDSP.output.CopyTo(voiceDSP.vEcho.input);
    int vEchoProcessed = voiceDSP.vEcho.processor->Process(state, ampEnvProcessed);
    voiceFinished = vEchoProcessed != FBFixedBlockSamples;
    voiceDSP.vEcho.output.CopyTo(voiceDSP.output);
  }

  _firstRoundThisVoice = false;

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    procState->dsp.voice[voice].vMatrix.processor->EndModulationBlock(state);
    return voiceFinished;
  }

  auto& exchangeDSP = exchangeToGUI->voice[voice].vMix[0];
  exchangeDSP.boolIsActive = 1;

  auto& exchangeParams = exchangeToGUI->param.voice.vMix[0];
  exchangeParams.acc.bal[0][voice] = balNormModulated.Last();
  exchangeParams.acc.amp[0][voice] = ampNormModulated.Last();
  exchangeParams.acc.lfo6ToBal[0][voice] = lfo6ToBalNorm.Last();
  exchangeParams.acc.ampEnvToAmp[0][voice] = ampEnvToAmpNorm.Last();
  exchangeParams.acc.osciMixToOut[0][voice] = osciMixToOutNorm.Last();
  for (int r = 0; r < FFMixFXToFXCount; r++)
    exchangeParams.acc.VFXToVFX[r][voice] = vMix.acc.VFXToVFX[r].Voice()[voice].CV().Last();
  for (int r = 0; r < FFVMixOsciToVFXCount; r++)
    exchangeParams.acc.osciToVFX[r][voice] = vMix.acc.osciToVFX[r].Voice()[voice].CV().Last();
  for (int r = 0; r < FFEffectCount; r++)
  {
    exchangeParams.acc.VFXToOut[r][voice] = vMix.acc.VFXToOut[r].Voice()[voice].CV().Last();
    exchangeParams.acc.osciMixToVFX[r][voice] = vMix.acc.osciMixToVFX[r].Voice()[voice].CV().Last();
  }
  for (int r = 0; r < FFOsciCount; r++)
  {
    exchangeParams.acc.osciToOut[r][voice] = vMix.acc.osciToOut[r].Voice()[voice].CV().Last();
    exchangeParams.acc.osciToOsciMix[r][voice] = vMix.acc.osciToOsciMix[r].Voice()[voice].CV().Last();
  }

  procState->dsp.voice[voice].vMatrix.processor->EndModulationBlock(state);
  return voiceFinished;
}