#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

template void 
FFRenderModuleGraph<false>(
  FFModuleGraphRenderData<FFEnvProcessor>&, 
  std::vector<float>&);
template void
FFRenderModuleGraph<true>(
  FFModuleGraphRenderData<FFGLFOProcessor>&,
  std::vector<float>&);

template <bool Global, class Processor>
void
FFRenderModuleGraph(
  FFModuleGraphRenderData<Processor>& renderData, 
  std::vector<float>& seriesOut)
{
  seriesOut.clear();
  FBFixedFloatArray seriesIn;
  int processed = FBFixedBlockSamples;
  auto renderState = renderData.graphData->renderState;
  auto const& moduleState = renderState->ModuleState();
  int moduleSlot = moduleState.moduleSlot;
  auto* procState = moduleState.template ProcState<FFProcState>();

  if constexpr(!Global)
    renderData.processor.BeginVoice(moduleState);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.processor.Process(moduleState);
    if constexpr(Global)
      renderData.globalOutputSelector(procState->dsp, moduleSlot)->StoreToFloatArray(seriesIn);
    else
      renderData.voiceOutputSelector(procState->dsp, moduleState.voice->slot, moduleSlot)->StoreToFloatArray(seriesIn);
    for (int i = 0; i < processed; i++)
      seriesOut.push_back(seriesIn.data[i]);
  }
}