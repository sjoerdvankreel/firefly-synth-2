#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

std::string
FFMakeRuntimeParamId(
  std::string const& staticModuleId, int moduleSlot,
  std::string const& staticParamId, int paramSlot)
{
  auto paramId = staticParamId + "-" + std::to_string(paramSlot);
  auto moduleId = staticModuleId + "-" + std::to_string(moduleSlot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FBStaticTopo const& topo,
  FBStaticModule const& module,
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices)
{
  auto paramName = FBMakeRuntimeShortName(topo, param.name, param.slotCount, indices.param.slot, param.slotFormatter, param.slotFormatterOverrides);
  auto moduleName = FBMakeRuntimeShortName(topo, module.name, module.slotCount, indices.module.slot, {}, false);
  return moduleName + " " + paramName;
}

FBRuntimeParamBase::
FBRuntimeParamBase(
  FBStaticTopo const& topo,
  FBStaticModule const& staticModule,
  FBStaticParamBase const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
runtimeModuleIndex(runtimeModuleIndex),
runtimeParamIndex(runtimeParamIndex),
topoIndices(topoIndices),
longName(MakeRuntimeParamLongName(topo, staticModule, staticParam, topoIndices)),
shortName(FBMakeRuntimeShortName(topo, staticParam.name, staticParam.slotCount, topoIndices.param.slot, staticParam.slotFormatter, staticParam.slotFormatterOverrides)),
displayName(FBMakeRuntimeDisplayName(topo, staticParam.name, staticParam.display, staticParam.slotCount, topoIndices.param.slot, staticParam.slotFormatter, staticParam.slotFormatterOverrides)),
id(FFMakeRuntimeParamId(staticModule.id, topoIndices.module.slot, staticParam.id, topoIndices.param.slot)),
staticModuleId(staticModule.id),
tag(FBMakeStableHash(id)) {}

std::string 
FBRuntimeParamBase::PlainToText(bool io, double plain) const
{
  return Static().NonRealTime().PlainToText(io, topoIndices.module.index, plain);
}

std::string
FBRuntimeParamBase::NormalizedToText(bool io, double normalized) const
{
  return Static().NonRealTime().NormalizedToText(io, topoIndices.module.index, normalized);
}

std::string 
FBRuntimeParamBase::NormalizedToTextWithUnit(bool io, double normalized) const
{
  return Static().NormalizedToTextWithUnit(io, topoIndices.module.index, normalized);
}

std::optional<double> 
FBRuntimeParamBase::TextToPlain(bool io, std::string const& text) const
{
  return Static().TextToPlain(io, topoIndices.module.index, text);
}

std::optional<double> 
FBRuntimeParamBase::TextToNormalized(bool io, std::string const& text) const
{
  return Static().TextToNormalized(io, topoIndices.module.index, text);
}

FBRuntimeGUIParam::
FBRuntimeGUIParam(
  FBStaticTopo const& topo,
  FBStaticModule const& staticModule,
  FBStaticGUIParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
FBRuntimeParamBase(topo, staticModule, staticParam, topoIndices, runtimeModuleIndex, runtimeParamIndex),
static_(staticParam) {}

FBRuntimeParam::
FBRuntimeParam(
  FBStaticTopo const& topo,
  FBStaticModule const& staticModule,
  FBStaticParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
FBRuntimeParamBase(topo, staticModule, staticParam, topoIndices, runtimeModuleIndex, runtimeParamIndex),
static_(staticParam)
{
#ifndef NDEBUG
  std::set<std::string> itemNames = {};
  if(staticParam.type == FBParamType::List)
    for (int i = 0; i < staticParam.List().items.size(); i++)
      FB_ASSERT(itemNames.insert(staticParam.List().GetName(topoIndices.module.index, i)).second);
#endif
}

std::string
FBRuntimeParam::GetDefaultText() const
{
  return static_.GetDefaultText(topoIndices.module.index, topoIndices.module.slot, topoIndices.param.slot);
}

std::string
FBRuntimeGUIParam::GetDefaultText() const
{
  return static_.GetDefaultText(topoIndices.module.index, topoIndices.module.slot, topoIndices.param.slot);
}

double
FBRuntimeParam::DefaultNormalizedByText() const
{
  return static_.DefaultNormalizedByText(topoIndices.module.index, topoIndices.module.slot, topoIndices.param.slot);
}

double
FBRuntimeGUIParam::DefaultNormalizedByText() const
{
  return static_.DefaultNormalizedByText(topoIndices.module.index, topoIndices.module.slot, topoIndices.param.slot);
}