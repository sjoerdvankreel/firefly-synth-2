#pragma once

struct FBModuleGraphComponentData;

// A bit unlike the others, but i want to reuse the exchange 
// state updating etc. So, it's got some custom processing.
void
FFGlobalUniRenderGraph(FBModuleGraphComponentData* graphData);