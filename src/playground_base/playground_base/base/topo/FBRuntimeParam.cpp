#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

static std::string
MakeRuntimeParamId(
  FBStaticModule const& module, int moduleSlot,
  FBStaticParam const& param, int paramSlot)
{
  auto moduleId = module.id + "-" + std::to_string(moduleSlot);
  auto paramId = param.id + "-" + std::to_string(paramSlot);
  return moduleId + "-" + paramId;
}

static int
MakeRuntimeHash(
  std::string const& id)
{
  std::uint32_t result = 0;
  int const multiplier = 33;
  for (char c : id)
    result = multiplier * result + static_cast<std::uint32_t>(c);
  return std::abs(static_cast<int>(result + (result >> 5)));
}

static std::string
MakeRuntimeParamLongName(
  FBStaticModule const& module, int moduleSlot,
  FBStaticParam const& param, int paramSlot)
{
  auto moduleName = FBMakeRuntimeName(module.name, module.slotCount, moduleSlot);
  auto paramName = FBMakeRuntimeName(param.name, param.slotCount, paramSlot);
  return moduleName + " " + paramName;
}

FBRuntimeParam::
FBRuntimeParam(
FBStaticModule const& module, int moduleSlot,
FBStaticParam const& param, int paramSlot) :
moduleSlot(moduleSlot),
paramSlot(paramSlot),
static_(param),
longName(MakeRuntimeParamLongName(module, moduleSlot, param, paramSlot)),
shortName(FBMakeRuntimeName(param.name, param.slotCount, paramSlot)),
id(MakeRuntimeParamId(module, moduleSlot, param, paramSlot)),
tag(MakeRuntimeHash(id)) {}