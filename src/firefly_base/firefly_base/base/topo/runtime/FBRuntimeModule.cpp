#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeModule.hpp>

static std::vector<FBRuntimeCVOutput>
MakeRuntimeCVOutputs(
  FBStaticTopo const& topo,
  FBStaticModule const& staticModule, 
  FBTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeCVOutputStart,
  std::vector<FBStaticCVOutput> const& staticCVOutputs)
{
  std::vector<FBRuntimeCVOutput> result;
  for (int o = 0; o < staticCVOutputs.size(); o++)
    for (int s = 0; s < staticCVOutputs[o].slotCount; s++)
    {
      FBCVOutputTopoIndices indices = {};
      indices.cvOutput.slot = s;
      indices.cvOutput.index = o;
      indices.module = topoIndices;
      result.push_back(FBRuntimeCVOutput(
        topo, staticModule, staticCVOutputs[o],
        indices, runtimeModuleIndex, runtimeCVOutputStart++));
    }
  return result;
}

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
  int runtimeParamStart, int runtimeGUIParamStart,
  int runtimeCVOutputStart):
name(FBMakeRuntimeShortName(topo, staticModule.name, staticModule.slotCount, topoIndices.slot, {}, false)),
runtimeModuleIndex(runtimeIndex),
topoIndices(topoIndices),
params(MakeRuntimeParams<FBRuntimeParam>(topo, staticModule, topoIndices, runtimeIndex, runtimeParamStart, staticModule.params)),
guiParams(MakeRuntimeParams<FBRuntimeGUIParam>(topo, staticModule, topoIndices, runtimeIndex, runtimeGUIParamStart, staticModule.guiParams)),
cvOutputs(MakeRuntimeCVOutputs(topo, staticModule, topoIndices, runtimeIndex, runtimeCVOutputStart, staticModule.cvOutputs))
{
  assert(staticModule.name.size());
#ifndef NDEBUG
  std::set<std::string> paramNames = {};
  for (int p = 0; p < staticModule.params.size(); p++)
    FB_ASSERT(paramNames.insert(staticModule.params[p].name).second);
  std::set<std::string> cvOutputNames = {};
  for (int o = 0; o < staticModule.cvOutputs.size(); o++)
    FB_ASSERT(cvOutputNames.insert(staticModule.cvOutputs[o].name).second);
#endif
}