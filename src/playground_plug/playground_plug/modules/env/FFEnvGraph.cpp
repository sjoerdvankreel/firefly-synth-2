#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

void
FFEnvRenderGraph(FBModuleGraphComponentData* data)
{
  int finishedAt = -1;
  FFEnvProcessor processor;
  FBFixedFloatArray points;
  auto const& moduleState = data->state->moduleState;
  auto* procState = moduleState.ProcState<FFProcState>();

  data->text = "ENV";
  data->state->moduleState.sampleRate = 100;
  processor.BeginVoice(moduleState);
  while (finishedAt == -1)
  {
    finishedAt = processor.Process(moduleState);
    procState->dsp.voice[0].env[moduleState.moduleSlot].output.StoreToFloatArray(points);
    for (int i = 0; i < FBFixedBlockSamples; i++)
      if(finishedAt == -1 || i <= finishedAt)
        data->points.push_back(points.data[i]);
  }
}