#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

template void 
FFRenderModuleGraph(FFModuleGraphRenderData<FFEnvProcessor>&);

template <class Processor>
void
FFRenderModuleGraph(FFModuleGraphRenderData<Processor>& renderData)
{
  FBFixedFloatArray points;
  int processed = FBFixedBlockSamples;
  auto const& moduleState = renderData.graphData->state->moduleState;
  auto* procState = moduleState.ProcState<FFProcState>();

  renderData.processor.BeginVoice(moduleState);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.processor.Process(moduleState);
    renderData.outputSelector(procState->dsp, moduleState.moduleSlot)->StoreToFloatArray(points);
    for (int i = 0; i < processed; i++)
      renderData.graphData->points.push_back(points.data[i]);
  }
}