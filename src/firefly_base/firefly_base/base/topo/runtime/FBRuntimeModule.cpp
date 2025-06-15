#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeModule.hpp>

template <class RuntimeParam, class StaticParam>
static std::vector<RuntimeParam>
MakeRuntimeParams(
  FBStaticTopo const& topo,
  FBStaticModule const& staticModule, 
  FBTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamStart,
  std::vector<StaticParam> const& staticParams)
{
  std::vector<RuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[p].slotCount; s++)
    {
      FBParamTopoIndices indices = {};
      indices.param.slot = s;
      indices.param.index = p;
      indices.module = topoIndices;
      result.push_back(RuntimeParam(
        topo, staticModule, staticParams[p],
        indices, runtimeModuleIndex, runtimeParamStart++));
    }
  return result;
}

FBRuntimeModule::
FBRuntimeModule(
  FBStaticTopo const& topo, FBStaticModule const& staticModule, 
  FBTopoIndices const& topoIndices, int runtimeIndex, 
  int runtimeParamStart, int runtimeGUIParamStart):
name(FBMakeRuntimeShortName(topo, staticModule.name, staticModule.slotCount, topoIndices.slot, {})),
tabName(FBMakeRuntimeGraphName(staticModule.tabName, staticModule.slotCount, topoIndices.slot)),
graphName(FBMakeRuntimeGraphName(staticModule.graphName, staticModule.slotCount, topoIndices.slot)),
runtimeModuleIndex(runtimeIndex),
topoIndices(topoIndices),
params(MakeRuntimeParams<FBRuntimeParam>(topo, staticModule, topoIndices, runtimeIndex, runtimeParamStart, staticModule.params)),
guiParams(MakeRuntimeParams<FBRuntimeGUIParam>(topo, staticModule, topoIndices, runtimeIndex, runtimeGUIParamStart, staticModule.guiParams))
{
  assert(staticModule.name.size());
  assert(staticModule.tabName.size());
  assert(staticModule.graphCount == 0 || staticModule.graphName.size());
#ifndef NDEBUG
  std::set<std::string> paramNames = {};
  for (int p = 0; p < staticModule.params.size(); p++)
    FB_ASSERT(paramNames.insert(staticModule.params[p].name).second);
#endif
}