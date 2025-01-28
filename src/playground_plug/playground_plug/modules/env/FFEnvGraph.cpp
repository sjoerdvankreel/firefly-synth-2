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
  FFEnvProcessor processor;
  data->text = "ENV";
  data->state->moduleState.sampleRate = 100;
  processor.BeginVoice(data->state->moduleState);

  int finishedAt = -1;
  while (finishedAt == -1)
  {
    auto* procState = data->state->moduleState.ProcState<FFProcState>();
    finishedAt = processor.Process(data->state->moduleState);
    auto& renderThis = procState->dsp.voice[0].env[data->state->moduleState.moduleSlot].output;
    FBFixedFloatArray rtArray = {};
    renderThis.StoreToFloatArray(rtArray);
    for (int i = 0; i < FBFixedBlockSamples; i++)
      if(finishedAt == -1 || i <= finishedAt)
        data->points.push_back(rtArray.data[i]);
  }
}