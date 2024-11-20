#include <playground_plug/base/FFBaseTopo.hpp>
#include <cstdint>

static std::string
MakeRuntimeName(
  std::string const& staticName, int slotCount, int slot)
{
  std::string result = staticName;
  if (slotCount > 1)
    result += std::to_string(slot + 1);
  return result;
}

int
MakeRuntimeParamHash(std::string const& id)
{
  std::uint32_t result = 0;
  int const multiplier = 33;
  for (char c : id)
    result = multiplier * result + static_cast<std::uint32_t>(c);
  return std::abs(static_cast<int>(result + (result >> 5)));
}

static std::string
MakeRuntimeParamId(
  FFStaticModule const& staticModule, int moduleSlot, 
  FFStaticParam const& staticParam, int paramSlot)
{
  auto moduleId = staticModule.id + "-" + std::to_string(moduleSlot);
  auto paramId = staticParam.id + "-" + std::to_string(paramSlot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FFStaticModule const& staticModule, int moduleSlot,
  FFStaticParam const& staticParam, int paramSlot)
{
  auto moduleName = MakeRuntimeName(staticModule.name, staticModule.slotCount, moduleSlot);
  auto paramName = MakeRuntimeName(staticParam.name, staticParam.slotCount, paramSlot);
  return moduleName + " " + paramName;
}

static std::vector<FFRuntimeParam>
MakeRuntimeParams(
  FFStaticModule const& staticModule, int moduleSlot, 
  std::vector<FFStaticParam> const& staticParams)
{
  std::vector<FFRuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[s].slotCount; s++)
      result.push_back(FFRuntimeParam(staticModule, moduleSlot, staticParams[p], s));
  return result;
}

FFRuntimeParam::
FFRuntimeParam(
  FFStaticModule const& staticModule, int moduleSlot,
  FFStaticParam const& staticParam, int paramSlot) :
moduleSlot(moduleSlot),
paramSlot(paramSlot),
staticParam(staticParam),
longName(MakeRuntimeParamLongName(staticModule, moduleSlot, staticParam, paramSlot)),
shortName(MakeRuntimeName(staticParam.name, staticParam.slotCount, paramSlot)),
id(MakeRuntimeParamId(staticModule, moduleSlot, staticParam, paramSlot)),
tag(MakeRuntimeParamHash(id)) {}

FFRuntimeModule::
FFRuntimeModule(
  FFStaticModule const& staticModule, int moduleSlot):
name(MakeRuntimeName(staticModule.name, staticModule.slotCount, moduleSlot)),
accParams(MakeRuntimeParams(staticModule, moduleSlot, staticModule.accParams)),
blockParams(MakeRuntimeParams(staticModule, moduleSlot, staticModule.blockParams)) {}

FFRuntimeTopo::
FFRuntimeTopo(FFStaticTopo const& staticTopo)
{
  for (int mi = 0; mi < topo.modules.size(); mi++)
    for (int ms = 0; ms < topo.modules[mi].slotCount; ms++)
      modules.push_back(FBRuntimeModule(topo.modules[mi], ms));
  for (int m = 0; m < modules.size(); m++)
    for (int pp = 0; pp < modules[m].plugParams.size(); pp++)
      plugParams.push_back(modules[m].plugParams[pp]);
  for (int m = 0; m < modules.size(); m++)
    for (int ap = 0; ap < modules[m].autoParams.size(); ap++)
      autoParams.push_back(modules[m].autoParams[ap]);
  for (int pp = 0; pp < plugParams.size(); pp++)
    tagToPlugParam[plugParams[pp].tag] = pp;
  for (int ap = 0; ap < autoParams.size(); ap++)
    tagToAutoParam[autoParams[ap].tag] = ap;
}