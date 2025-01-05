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
  FBStaticModule const& staticModule,
  FBStaticParam const& staticParam,
  int runtimeModuleIndex, int runtimeParamIndex,
  int staticModuleIndex, int staticModuleSlot,
  int staticIndex, int staticSlot):
runtimeModuleIndex(runtimeModuleIndex),
runtimeParamIndex(runtimeParamIndex),
staticModuleIndex(staticModuleIndex),
staticModuleSlot(staticModuleSlot),
staticIndex(staticIndex),
staticSlot(staticSlot),
static_(staticParam),
longName(MakeRuntimeParamLongName(staticModule, staticModuleSlot, staticParam, staticSlot)),
shortName(FBMakeRuntimeName(staticParam.name, staticParam.slotCount, staticSlot)),
id(MakeRuntimeParamId(staticModule, staticModuleSlot, staticParam, staticSlot)),
tag(FBMakeStableHash(id)) {}