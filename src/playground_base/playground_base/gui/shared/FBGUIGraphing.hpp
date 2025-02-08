#pragma once

#include <vector>
#include <functional>

class FBFixedFloatBlock;
struct FBModuleGraphComponentData;

typedef std::function<FBFixedFloatBlock const* (
  void const* procState, int moduleSlot)>
FBModuleGraphGlobalOutputSelector;
typedef std::function<FBFixedFloatBlock const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceOutputSelector;

template <class Processor>
struct FBModuleGraphRenderData final
{
  Processor processor = {};
  FBModuleGraphComponentData* graphData = {};
  FBModuleGraphVoiceOutputSelector voiceOutputSelector = {};
  FBModuleGraphGlobalOutputSelector globalOutputSelector = {};
};

template <bool Global, class Processor> 
void 
FBRenderModuleGraphSeries(
  FBModuleGraphRenderData<Processor>& renderData, 
  std::vector<float>& seriesOut)
{
  seriesOut.clear();
  FBFixedFloatArray seriesIn;
  int processed = FBFixedBlockSamples;
  auto renderState = renderData.graphData->renderState;
  auto const& moduleState = renderState->ModuleState();
  int moduleSlot = moduleState.moduleSlot;

  if constexpr (Global)
    renderData.processor.Reset(moduleState);
  else
    renderData.processor.BeginVoice(moduleState);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.processor.Process(moduleState);
    if constexpr(Global)
      renderData.globalOutputSelector(moduleState.procRaw, moduleSlot)->StoreToFloatArray(seriesIn);
    else
      renderData.voiceOutputSelector(moduleState.procRaw, moduleState.voice->slot, moduleSlot)->StoreToFloatArray(seriesIn);
    for (int i = 0; i < processed; i++)
      seriesOut.push_back(seriesIn.data[i]);
  }
}