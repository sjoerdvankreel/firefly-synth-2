#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

static std::string
MakeRuntimeParamId(
  FBStaticModule const& module, 
  FBStaticParam const& param,
  FBParamTopoIndices const& indices)
{
  auto paramId = param.id + "-" + std::to_string(indices.param.slot);
  auto moduleId = module.id + "-" + std::to_string(indices.module.slot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FBStaticModule const& module,
  FBStaticParam const& param,
  FBParamTopoIndices const& indices)
{
  auto paramName = FBMakeRuntimeName(param.name, param.slotCount, indices.param.slot);
  auto moduleName = FBMakeRuntimeName(module.name, module.slotCount, indices.module.slot);
  return moduleName + " " + paramName;
}

FBRuntimeParam::
FBRuntimeParam(
  FBStaticModule const& staticModule,
  FBStaticParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
runtimeModuleIndex(runtimeModuleIndex),
runtimeParamIndex(runtimeParamIndex),
static_(staticParam),
topoIndices(topoIndices),
longName(MakeRuntimeParamLongName(staticModule, staticParam, topoIndices)),
shortName(FBMakeRuntimeName(staticParam.name, staticParam.slotCount, topoIndices.param.slot)),
tooltip(FBMakeRuntimeTooltip(staticParam.name, staticParam.tooltip, staticParam.slotCount, topoIndices.param.slot)),
id(MakeRuntimeParamId(staticModule, staticParam, topoIndices)),
tag(FBMakeStableHash(id)) {}