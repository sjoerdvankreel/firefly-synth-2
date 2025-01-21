#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>

static std::vector<FBRuntimeParam>
MakeRuntimeParams(
  FBStaticModule const& staticModule, 
  FBTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamStart,
  std::vector<FBStaticParam> const& staticParams)
{
  std::vector<FBRuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[p].slotCount; s++)
    {
      FBParamTopoIndices indices = {};
      indices.param.slot = s;
      indices.param.index = p;
      indices.module = topoIndices;
      result.push_back(FBRuntimeParam(
        staticModule, staticParams[p],
        indices, runtimeModuleIndex, runtimeParamStart++));
    }
  return result;
}

FBRuntimeModule::
FBRuntimeModule(
  FBStaticModule const& staticModule, 
  FBTopoIndices const& topoIndices,
  int runtimeIndex, int runtimeParamStart):
topoIndices(topoIndices),
name(FBMakeRuntimeName(staticModule.name, staticModule.slotCount, topoIndices.slot)),
params(MakeRuntimeParams(staticModule, topoIndices, runtimeIndex, runtimeParamStart, staticModule.params)) {}