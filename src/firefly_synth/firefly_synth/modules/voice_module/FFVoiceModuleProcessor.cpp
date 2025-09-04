#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

void 
FFVoiceModuleProcessor::BeginVoice(
  FBModuleProcState& state,
  float previousMidiKeyUntuned, 
  bool anyNoteWasOnAlready)
{
  int voice = state.voice->slot;
  float bpm = state.input->bpm;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.voiceModule[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::VoiceModule];
 
  float portaTypeNorm = params.block.portaType[0].Voice()[voice];
  float portaModeNorm = params.block.portaMode[0].Voice()[voice];
  float portaSyncNorm = params.block.portaSync[0].Voice()[voice];
  float portaTimeNorm = params.block.portaTime[0].Voice()[voice];
  float portaBarsNorm = params.block.portaBars[0].Voice()[voice];
  float portaSectionAmpAttackNorm = params.block.portaSectionAmpAttack[0].Voice()[voice];

  bool portaSync = topo.NormalizedToBoolFast(FFVoiceModuleParam::PortaSync, portaSyncNorm);
  auto portaType = topo.NormalizedToListFast<FFVoiceModulePortaType>(FFVoiceModuleParam::PortaType, portaTypeNorm);
  auto portaMode = topo.NormalizedToListFast<FFVoiceModulePortaMode>(FFVoiceModuleParam::PortaMode, portaModeNorm);

  float portaPitchStart;
  if (previousMidiKeyUntuned < 0.0f
    || portaType == FFVoiceModulePortaType::Off
    || portaMode == FFVoiceModulePortaMode::Section && !anyNoteWasOnAlready)
  {
    portaPitchStart = (float)state.voice->event.note.keyUntuned;
    procState->dsp.voice[voice].voiceModule.thisVoicePortaSectionAttackMultiplier = 1.0f;
  }
  else
  {
    portaPitchStart = previousMidiKeyUntuned;
    procState->dsp.voice[voice].voiceModule.thisVoicePortaSectionAttackMultiplier = portaSectionAmpAttackNorm;
  }

  if (portaSync)
    _portaPitchSamplesTotal = topo.NormalizedToBarsSamplesFast(
      FFVoiceModuleParam::PortaBars, portaBarsNorm, sampleRate, bpm);
  else
    _portaPitchSamplesTotal = topo.NormalizedToLinearTimeSamplesFast(
      FFVoiceModuleParam::PortaTime, portaTimeNorm, sampleRate);

  int portaDiffSemis = state.voice->event.note.keyUntuned - (int)std::round(portaPitchStart);
  int slideMultiplier = portaType == FFVoiceModulePortaType::Auto ? 1 : std::abs(portaDiffSemis);
  _portaPitchSamplesTotal *= slideMultiplier;
  _portaPitchSamplesProcessed = 0;
  if (_portaPitchSamplesTotal == 0)
  {
    _portaPitchDelta = 0.0f;
    _portaPitchOffsetCurrent = 0.0f;
  }
  else
  {
    _portaPitchDelta = portaDiffSemis / (float)_portaPitchSamplesTotal;
    _portaPitchOffsetCurrent = (float)portaDiffSemis;
  }
}

void
FFVoiceModuleProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto const& procParams = procState->param.voice.voiceModule[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::VoiceModule];
  auto& pitchOffsetInSemis = voiceState.voiceModule.pitchOffsetInSemis;

  auto const& fineNormIn = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNormIn = procParams.acc.coarse[0].Voice()[voice];
  auto const& lfo5ToFine = procParams.acc.lfo5ToFine[0].Voice()[voice];
  auto const& env5ToCoarse = procParams.acc.env5ToCoarse[0].Voice()[voice];
  FBSArray<float, FBFixedBlockSamples> fineNormModulated = {};
  FBSArray<float, FBFixedBlockSamples> coarseNormModulated = {};

  fineNormIn.CV().CopyTo(fineNormModulated);
  coarseNormIn.CV().CopyTo(coarseNormModulated);
  FFApplyModulation(FFModulationOpType::UPStack, voiceState.env[FFEnvSlotOffset + 4].output, env5ToCoarse.CV(), coarseNormModulated);
  FFApplyModulation(FFModulationOpType::BPStack, voiceState.vLFO[4].outputAll, lfo5ToFine.CV(), fineNormModulated);

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto coarsePlain = topo.NormalizedToLinearFast(FFVoiceModuleParam::Coarse, coarseNormModulated.Load(s));
    auto finePlain = topo.NormalizedToLinearFast(FFVoiceModuleParam::Fine, fineNormModulated.Load(s));
    pitchOffsetInSemis.Store(s, coarsePlain + finePlain);
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    if(_portaPitchSamplesProcessed++ <= _portaPitchSamplesTotal)
      _portaPitchOffsetCurrent -= _portaPitchDelta;
    pitchOffsetInSemis.Set(s, pitchOffsetInSemis.Get(s) - _portaPitchOffsetCurrent);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->voice[voice].voiceModule[0];
  exchangeDSP.active = true;

  auto& exchangeParams = exchangeToGUI->param.voice.voiceModule[0];
  exchangeParams.acc.coarse[0][voice] = coarseNormModulated.Last();
  exchangeParams.acc.env5ToCoarse[0][voice] = env5ToCoarse.Last();
  exchangeParams.acc.fine[0][voice] = fineNormModulated.Last();
  exchangeParams.acc.lfo5ToFine[0][voice] = lfo5ToFine.Last();
}