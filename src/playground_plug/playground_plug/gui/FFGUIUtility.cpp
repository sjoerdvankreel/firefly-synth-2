#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

template void 
FFRenderModuleGraph(
  FFModuleGraphRenderData<FFEnvProcessor>&, 
  std::vector<float>&);

template <class Processor>
void
FFRenderModuleGraph(
  FFModuleGraphRenderData<Processor>& renderData, 
  std::vector<float>& pointsOut)
{
  // TODO tidy

  pointsOut.clear();
  FBFixedFloatArray pointsIn;
  int processed = FBFixedBlockSamples;
  auto const& moduleState = renderData.graphData->state->ModuleState();
  auto* procState = moduleState.ProcState<FFProcState>();

  renderData.processor.BeginVoice(moduleState);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.processor.Process(moduleState);
    renderData.outputSelector(procState->dsp, renderData.graphData->state->ModuleState().voice->slot, moduleState.moduleSlot)->StoreToFloatArray(pointsIn);
    for (int i = 0; i < processed; i++)
      pointsOut.push_back(pointsIn.data[i]);
  }
}