#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

static void
ValidateItemsParam(
  FBStaticParamBase const& staticParam, 
  int moduleIndex)
{
  (void)moduleIndex;
  (void)staticParam;
#ifndef NDEBUG
  std::set<std::string> itemNames = {};
  if (staticParam.type == FBParamType::List)
    for (int i = 0; i < staticParam.List().items.size(); i++)
    {
      auto name = staticParam.List().GetName(moduleIndex, i);
      FB_ASSERT(itemNames.insert(name).second);
    }
#endif
}

std::string
FBMakeRuntimeParamLongName(
  FBStaticTopo const& topo,
  FBStaticModule const& module,
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices)
{
  auto paramName = FBMakeRuntimeModuleItemShortName(
    topo, param.name, indices.module.slot, param.slotCount, 
    indices.param.slot, param.slotFormatter, param.slotFormatterOverrides);
  auto moduleName = FBMakeRuntimeModuleShortName(
    topo, module.name, module.slotCount, indices.module.slot, 
    module.slotFormatter, module.slotFormatterOverrides);
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
longName(FBMakeRuntimeParamLongName(topo, staticModule, staticParam, topoIndices)),
shortName(FBMakeRuntimeModuleItemShortName(
  topo, staticParam.name, topoIndices.module.slot, staticParam.slotCount, 
  topoIndices.param.slot, staticParam.slotFormatter, staticParam.slotFormatterOverrides)),
displayName(FBMakeRuntimeModuleItemDisplayName(
  topo, staticParam.name, staticParam.display, 
  topoIndices.module.slot, staticParam.slotCount, topoIndices.param.slot, 
  staticParam.slotFormatter, staticParam.slotFormatterOverrides, staticParam.slotFormatDisplay)),
id(FBMakeRuntimeId(staticModule.id, topoIndices.module.slot, staticParam.id, topoIndices.param.slot)),
staticModuleId(staticModule.id),
tag(FBMakeStableHash(id))
{
  FB_ASSERT((staticParam.defaultText.size() == 0) != (staticParam.defaultTextSelector == nullptr));
#ifndef NDEBUG
  staticParam.GetDefaultText(topoIndices.module.index, topoIndices.module.slot, topoIndices.param.slot);
#endif
}

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
static_(staticParam)
{
  ValidateItemsParam(staticParam, topoIndices.module.index);
}

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
  ValidateItemsParam(staticParam, topoIndices.module.index);
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