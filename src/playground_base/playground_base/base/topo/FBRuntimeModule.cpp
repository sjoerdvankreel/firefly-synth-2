#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>

static std::vector<FBRuntimeParam>
MakeRuntimeParams(
  FBStaticModule const& staticModule, int runtimeModuleIndex, 
  int staticModuleIndex, int staticModuleSlot,
  std::vector<FBStaticParam> const& staticParams)
{
  std::vector<FBRuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[p].slotCount; s++)
      result.push_back(FBRuntimeParam(
        staticModule, staticParams[p], runtimeModuleIndex, staticModuleIndex, staticModuleSlot, p, s));
  return result;
}

FBRuntimeModule::
FBRuntimeModule(
  FBStaticModule const& staticModule, int runtimeIndex,
  int staticIndex, int staticSlot) :
staticIndex(staticIndex),
staticSlot(staticSlot),
name(FBMakeRuntimeName(staticModule.name, staticModule.slotCount, staticSlot)),
params(MakeRuntimeParams(staticModule, runtimeIndex, staticIndex, staticSlot, staticModule.params)) {}