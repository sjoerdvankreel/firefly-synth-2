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
  std::vector<float>& seriesOut)
{
  seriesOut.clear();
  FBFixedFloatArray seriesIn;
  int processed = FBFixedBlockSamples;
  int voice = renderData.graphData->state->ModuleState().voice->slot;
  auto const& moduleState = renderData.graphData->state->ModuleState();
  auto* procState = moduleState.ProcState<FFProcState>();

  renderData.processor.BeginVoice(moduleState);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.processor.Process(moduleState);
    renderData.outputSelector(procState->dsp, voice, moduleState.moduleSlot)->StoreToFloatArray(seriesIn);
    for (int i = 0; i < processed; i++)
      seriesOut.push_back(seriesIn.data[i]);
  }
}