#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

void
FFEnvRenderGraph(FBModuleGraphComponentData* data)
{
  // TODO
  int wantSamples = 100;
  FFEnvProcessor processor;
  data->text = "ENV";
  processor.BeginVoice(data->state->moduleState);
  while (wantSamples > 0)
  {
    auto* procState = data->state->moduleState.ProcState<FFProcState>();
    processor.Process(data->state->moduleState);
    auto& renderThis = procState->dsp.voice[0].env[data->state->moduleState.moduleSlot].output;
    FBFixedFloatArray rtArray = {};
    renderThis.StoreToFloatArray(rtArray);
    for (int i = 0; i < FBFixedBlockSamples && wantSamples > 0; i++, wantSamples--)
      data->points.push_back(rtArray.data[i]);
  }
}