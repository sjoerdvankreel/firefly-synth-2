#pragma once

#include <functional>

struct FFProcDSPState;
struct FBModuleGraphComponentData;
class FBFixedFloatAudioBlock;

typedef std::function<FBFixedFloatAudioBlock const* (
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
FFRenderModuleGraph(FFModuleGraphRenderData<Processor>& renderData);