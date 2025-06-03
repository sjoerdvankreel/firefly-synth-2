#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

static std::string
MakeRuntimeParamId(
  FBStaticModule const& module, 
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices)
{
  auto paramId = param.id + "-" + std::to_string(indices.param.slot);
  auto moduleId = module.id + "-" + std::to_string(indices.module.slot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FBStaticModule const& module,
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices)
{
  auto paramName = FBMakeRuntimeShortName(param.name, param.slotCount, indices.param.slot, param.slotFormatter);
  auto moduleName = FBMakeRuntimeShortName(module.name, module.slotCount, indices.module.slot, {});
  return moduleName + " " + paramName;
}

FBRuntimeParamBase::
FBRuntimeParamBase(
  FBStaticModule const& staticModule,
  FBStaticParamBase const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
runtimeModuleIndex(runtimeModuleIndex),
runtimeParamIndex(runtimeParamIndex),
topoIndices(topoIndices),
longName(MakeRuntimeParamLongName(staticModule, staticParam, topoIndices)),
shortName(FBMakeRuntimeShortName(staticParam.name, staticParam.slotCount, topoIndices.param.slot, staticParam.slotFormatter)),
displayName(FBMakeRuntimeDisplayName(staticParam.name, staticParam.display, staticParam.slotCount, topoIndices.param.slot, staticParam.slotFormatter)),
id(MakeRuntimeParamId(staticModule, staticParam, topoIndices)),
tag(FBMakeStableHash(id)) {}

FBRuntimeGUIParam::
FBRuntimeGUIParam(
  FBStaticModule const& staticModule,
  FBStaticGUIParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
FBRuntimeParamBase(staticModule, staticParam, topoIndices, runtimeModuleIndex, runtimeParamIndex),
static_(staticParam) {}

FBRuntimeParam::
FBRuntimeParam(
  FBStaticModule const& staticModule,
  FBStaticParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
FBRuntimeParamBase(staticModule, staticParam, topoIndices, runtimeModuleIndex, runtimeParamIndex),
static_(staticParam)
{
#ifndef NDEBUG
  std::set<std::string> itemNames = {};
  if(staticParam.type == FBParamType::List)
    for (int i = 0; i < staticParam.List().items.size(); i++)
      assert(itemNames.insert(staticParam.List().items[i].name).second);
#endif
}

std::string
FBRuntimeParam::GetDefaultText() const
{
  return static_.GetDefaultText(topoIndices.module.slot, topoIndices.param.slot);
}

std::string
FBRuntimeGUIParam::GetDefaultText() const
{
  return static_.GetDefaultText(topoIndices.module.slot, topoIndices.param.slot);
}

double
FBRuntimeParam::DefaultNormalizedByText() const
{
  return static_.DefaultNormalizedByText(topoIndices.module.slot, topoIndices.param.slot);
}

double
FBRuntimeGUIParam::DefaultNormalizedByText() const
{
  return static_.DefaultNormalizedByText(topoIndices.module.slot, topoIndices.param.slot);
}