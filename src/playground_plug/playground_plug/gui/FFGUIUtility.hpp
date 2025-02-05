#pragma once

#include <vector>
#include <functional>

struct FFProcDSPState;
struct FBModuleGraphComponentData;
class FBFixedFloatBlock;

typedef std::function<FBFixedFloatBlock const* (
  FFProcDSPState const&, int moduleSlot)>
  FFModuleGraphGlobalOutputSelector;
typedef std::function<FBFixedFloatBlock const* (
  FFProcDSPState const&, int voice, int moduleSlot)>
FFModuleGraphVoiceOutputSelector;

template <class Processor>
struct FFModuleGraphRenderData final
{
  Processor processor = {};
  FBModuleGraphComponentData* graphData = {};
  FFModuleGraphVoiceOutputSelector voiceOutputSelector = {};
  FFModuleGraphGlobalOutputSelector globalOutputSelector = {};
};

template <class Processor> 
void 
FFRenderModuleGraph(
  FFModuleGraphRenderData<Processor>& renderData, 
  std::vector<float>& seriesOut);