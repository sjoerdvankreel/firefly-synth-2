#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>

static std::vector<FBRuntimeParam>
MakeRuntimeParams(
  FBStaticModule const& module, int slot,
  std::vector<FBStaticParam> const& params)
{
  std::vector<FBRuntimeParam> result;
  for (int p = 0; p < params.size(); p++)
    for (int s = 0; s < params[p].slotCount; s++)
      result.push_back(FBRuntimeParam(module, slot, params[p], s));
  return result;
}

FBRuntimeModule::
FBRuntimeModule(
FBStaticModule const& module, int moduleSlot) :
name(FBMakeRuntimeName(module.name, module.slotCount, moduleSlot)),
acc(MakeRuntimeParams(module, moduleSlot, module.acc)),
block(MakeRuntimeParams(module, moduleSlot, module.block)) {}