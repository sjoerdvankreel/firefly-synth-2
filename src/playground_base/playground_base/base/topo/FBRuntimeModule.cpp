#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>

static std::vector<FBRuntimeParam>
MakeRuntimeParams(
  FBStaticModule const& module, 
  int moduleIndex, int moduleSlot,
  std::vector<FBStaticParam> const& params)
{
  std::vector<FBRuntimeParam> result;
  for (int p = 0; p < params.size(); p++)
    for (int s = 0; s < params[p].slotCount; s++)
      result.push_back(FBRuntimeParam(module, moduleIndex, moduleSlot, params[p], s));
  return result;
}

FBRuntimeModule::
FBRuntimeModule(
FBStaticModule const& module, int moduleIndex, int moduleSlot) :
name(FBMakeRuntimeName(module.name, module.slotCount, moduleSlot)),
params(MakeRuntimeParams(module, moduleIndex, moduleSlot, module.params)) {}