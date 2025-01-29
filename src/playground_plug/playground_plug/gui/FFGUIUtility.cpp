#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>

#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

template <class Processor>
void
FFRenderModuleGraph(FFModuleGraphRenderData<Processor>& data)
{
  int finishedAt = -1;
  FBFixedFloatArray points;
  auto const& moduleState = data.graph->state->moduleState;
  auto* procState = moduleState.ProcState<FFProcState>();

  data.processor.BeginVoice(moduleState);
  while (finishedAt == -1)
  {
    finishedAt = data.processor.Process(moduleState);
    data.outputSelector(procState->dsp, moduleState.moduleSlot)->StoreToFloatArray(points);
    for (int i = 0; i < FBFixedBlockSamples; i++)
      if (finishedAt == -1 || i <= finishedAt)
        data->graph.points.push_back(points.data[i]);
  }
}