#pragma once

#include <vector>
#include <functional>

struct FFProcDSPState;
struct FBModuleGraphComponentData;
class FBFixedFloatBlock;

typedef std::function<FBFixedFloatBlock const* (
    FFProcDSPState const&, int moduleSlot)>
FFModuleGraphOutputSelector;

template <class Processor>
struct FFModuleGraphRenderData final
{
  Processor processor = {};
  FBModuleGraphComponentData* graphData = {};
  FFModuleGraphOutputSelector outputSelector = {};
};

template <class Processor> 
void 
FFRenderModuleGraph(
  FFModuleGraphRenderData<Processor>& renderData, 
  std::vector<float>& pointsOut);