#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>

static std::vector<FBRuntimeParam>
MakeRuntimeParams(
  FBStaticModule const& staticModule, 
  int runtimeModuleIndex, int runtimeParamStart,
  int staticModuleIndex, int staticModuleSlot,
  std::vector<FBStaticParam> const& staticParams)
{
  std::vector<FBRuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[p].slotCount; s++)
    {
      FBParamTopoIndices indices = {};
      indices.staticParamSlot = s;
      indices.staticParamIndex = p;
      indices.staticModuleSlot = staticModuleSlot;
      indices.staticModuleIndex = staticModuleIndex;
      result.push_back(FBRuntimeParam(
        staticModule, staticParams[p],
        indices, runtimeModuleIndex, runtimeParamStart++));
    }
  return result;
}

FBRuntimeModule::
FBRuntimeModule(
  FBStaticModule const& staticModule, 
  int runtimeIndex, int runtimeParamStart,
  int staticIndex, int staticSlot) :
staticIndex(staticIndex),
staticSlot(staticSlot),
name(FBMakeRuntimeName(staticModule.name, staticModule.slotCount, staticSlot)),
params(MakeRuntimeParams(staticModule, runtimeIndex, runtimeParamStart, staticIndex, staticSlot, staticModule.params)) {}