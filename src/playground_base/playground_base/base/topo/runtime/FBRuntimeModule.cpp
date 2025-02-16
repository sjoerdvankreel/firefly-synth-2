#include <playground_base/base/topo/static/FBStaticModule.hpp>
#include <playground_base/base/topo/runtime/FBTopoDetail.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeModule.hpp>

static std::vector<FBRuntimeAudioParam>
MakeRuntimeAudioParams(
  FBStaticModule const& staticModule, 
  FBTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamStart,
  std::vector<FBStaticAudioParam> const& staticParams)
{
  std::vector<FBRuntimeAudioParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[p].slotCount; s++)
    {
      FBParamTopoIndices indices = {};
      indices.param.slot = s;
      indices.param.index = p;
      indices.module = topoIndices;
      result.push_back(FBRuntimeAudioParam(
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
runtimeModuleIndex(runtimeIndex),
name(FBMakeRuntimeName(staticModule.name, staticModule.slotCount, topoIndices.slot)),
params(MakeRuntimeParams(staticModule, topoIndices, runtimeIndex, runtimeParamStart, staticModule.params)) {}