#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

void
FFEnvRenderGraph(
  FBPlugGUI const* plugGUI,
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

  
  // TODO this is bound to be slow
  // better to keep 1 fbprocstatecontainer in the graph
  // and here we go again with the mem usage
  FBProcStateContainer container(*plugGUI->Topo());
  for (int i = 0; i < plugGUI->Topo()->params.size(); i++)
    container.InitProcessing(i, plugGUI->HostContext()->GetParamNormalized(i));
  auto procState = static_cast<FFProcState*>(container.Raw());

  FFModuleProcState state;
  state.input = &input;
  state.moduleSlot = moduleSlot;
  state.voice = &voiceInfo;
  state.outputParamsNormalized = &outputParamsNormalized;
  state.topo = plugGUI->Topo();
  state.sampleRate = 100;
  state.proc = procState;

  // TODO
  int wantSamples = 100;
  FFEnvProcessor processor;
  data->text = "ENV";

  while (wantSamples > 0)
  {
    processor.Process(state);
    auto& renderThis = procState->dsp.voice[0].env[moduleSlot].output;
    FBFixedFloatArray rtArray = {};
    renderThis.StoreToFloatArray(rtArray);
    for (int i = 0; i < FBFixedBlockSamples && wantSamples > 0; i++, wantSamples--)
      data->points.push_back(rtArray.data[i]);
  }
}