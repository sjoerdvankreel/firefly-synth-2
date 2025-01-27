#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

void
FFGLFORenderGraph(
  FBRuntimeTopo const* topo,
  FBScalarStateContainer const* scalar,
  int moduleSlot,
  FBModuleGraphComponentData* data)
{
  // TODO
  // should we involve processor state here?
  // pros: looks fancy
  // cons: lots of work
  // cons: mem/cpu overhead
  // cons: needs more ui real estate since theres no point doing it with a single graph, needs at least 1 per module
  FBPlugInputBlock input = {};
  std::vector<FBNoteEvent> notes = {};
  FBFixedFloatAudioBlock inputAudio = {};
  input.note = &notes;
  input.audio = &inputAudio;
  input.voiceManager = nullptr;
  std::unordered_map<int, float> outputParamsNormalized = {};
  FBVoiceInfo voiceInfo = {};
  voiceInfo.event.note.channel = 0;
  voiceInfo.event.note.id = 0;
  voiceInfo.event.note.key = 60;
  voiceInfo.event.on = true;
  voiceInfo.event.pos = 0;
  voiceInfo.event.velo = 1.0f;
  voiceInfo.initialOffset = 0;
  voiceInfo.slot = 0;
  voiceInfo.state = FBVoiceState::Active;

  FBProcStateContainer container(*topo);
  container.InitProcessing(*scalar);
  auto procState = static_cast<FFProcState*>(container.Raw());

  FFModuleProcState state;
  state.input = &input;
  state.moduleSlot = moduleSlot;
  state.voice = &voiceInfo;
  state.outputParamsNormalized = &outputParamsNormalized;
  state.topo = topo;
  state.sampleRate = 100;
  state.proc = procState;

  // TODO
  int wantSamples = 100;
  FFGLFOProcessor processor;
  data->text = "ENV";

  while (wantSamples > 0)
  {
    processor.Process(state);
    auto& renderThis = procState->dsp.global.gLFO[moduleSlot].output;
    FBFixedFloatArray rtArray = {};
    renderThis.StoreToFloatArray(rtArray);
    for (int i = 0; i < FBFixedBlockSamples && wantSamples > 0; i++, wantSamples--)
      data->points.push_back(rtArray.data[i]);
  }
}