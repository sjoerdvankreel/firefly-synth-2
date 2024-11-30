#include <playground_base/shared/FBPlugTopo.hpp>
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
MakeRuntimeParamId(
  FBStaticModule const& staticModule, int moduleSlot,
  FBStaticParam const& staticParam, int paramSlot)
{
  auto moduleId = staticModule.id + "-" + std::to_string(moduleSlot);
  auto paramId = staticParam.id + "-" + std::to_string(paramSlot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FBStaticModule const& staticModule, int moduleSlot,
  FBStaticParam const& staticParam, int paramSlot)
{
  auto moduleName = MakeRuntimeName(staticModule.name, staticModule.slotCount, moduleSlot);
  auto paramName = MakeRuntimeName(staticParam.name, staticParam.slotCount, paramSlot);
  return moduleName + " " + paramName;
}

static std::map<int, int>
MakeTagToParam(
  std::vector<FBRuntimeParam> const& runtimeParams)
{
  std::map<int, int> result;
  for (int p = 0; p < runtimeParams.size(); p++)
    result[runtimeParams[p].tag] = p;
  return result;
}

static std::vector<FBRuntimeParam>
MakeRuntimeAcc(
  std::vector<FBRuntimeModule> const& runtimeModules)
{
  std::vector<FBRuntimeParam> result;
  for (int m = 0; m < runtimeModules.size(); m++)
    for (int ap = 0; ap < runtimeModules[m].acc.size(); ap++)
      result.push_back(runtimeModules[m].acc[ap]);
  return result;
}

static std::vector<FBRuntimeParam>
MakeRuntimeBlock(
  std::vector<FBRuntimeModule> const& runtimeModules)
{
  std::vector<FBRuntimeParam> result;
  for (int m = 0; m < runtimeModules.size(); m++)
    for (int bp = 0; bp < runtimeModules[m].block.size(); bp++)
      result.push_back(runtimeModules[m].block[bp]);
  return result;
}

static std::vector<FBRuntimeModule>
MakeRuntimeModules(FBStaticTopo const& staticTopo)
{
  std::vector<FBRuntimeModule> result;
  for (int mi = 0; mi < staticTopo.modules.size(); mi++)
    for (int ms = 0; ms < staticTopo.modules[mi].slotCount; ms++)
      result.push_back(FBRuntimeModule(staticTopo.modules[mi], ms));
  return result;
}

static std::vector<FBRuntimeParam>
MakeRuntimeParams(
  FBStaticModule const& staticModule, int moduleSlot,
  std::vector<FBStaticParam> const& staticParams)
{
  std::vector<FBRuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[p].slotCount; s++)
      result.push_back(FBRuntimeParam(staticModule, moduleSlot, staticParams[p], s));
  return result;
}

void 
FBRuntimeTopo::InitScalarAddrs(FBScalarAddrsBase& addrs) const
{
  addrs.acc.clear();
  for (int a = 0; a < acc.size(); a++)
    addrs.acc.push_back(acc[a].staticParam.scalarAddr(
      acc[a].moduleSlot, acc[a].paramSlot, addrs));

  addrs.block.clear();
  for (int b = 0; b < block.size(); b++)
    addrs.block.push_back(block[b].staticParam.scalarAddr(
      block[b].moduleSlot, block[b].paramSlot, addrs));
}

void 
FBRuntimeTopo::InitProcAddrs(FBProcAddrsBase& addrs) const
{
  addrs.pos.clear();
  for (int a = 0; a < acc.size(); a++)
    addrs.pos.push_back(acc[a].staticParam.posAddr(
      acc[a].moduleSlot, acc[a].paramSlot, addrs));

  addrs.cv.clear();
  for (int a = 0; a < acc.size(); a++)
    addrs.cv.push_back(acc[a].staticParam.cvAddr(
      acc[a].moduleSlot, acc[a].paramSlot, addrs));
}

FBRuntimeTopo::
FBRuntimeTopo(FBStaticTopo const& staticTopo):
modules(MakeRuntimeModules(staticTopo)),
acc(MakeRuntimeAcc(modules)),
block(MakeRuntimeBlock(modules)),
tagToAcc(MakeTagToParam(acc)),
tagToBlock(MakeTagToParam(block)) {}

FBRuntimeParam::
FBRuntimeParam(
  FBStaticModule const& staticModule, int moduleSlot,
  FBStaticParam const& staticParam, int paramSlot) :
moduleSlot(moduleSlot),
paramSlot(paramSlot),
staticParam(staticParam),
longName(MakeRuntimeParamLongName(staticModule, moduleSlot, staticParam, paramSlot)),
shortName(MakeRuntimeName(staticParam.name, staticParam.slotCount, paramSlot)),
id(MakeRuntimeParamId(staticModule, moduleSlot, staticParam, paramSlot)),
tag(MakeRuntimeHash(id)) {}

FBRuntimeModule::
FBRuntimeModule(
  FBStaticModule const& staticModule, int moduleSlot):
name(MakeRuntimeName(staticModule.name, staticModule.slotCount, moduleSlot)),
acc(MakeRuntimeParams(staticModule, moduleSlot, staticModule.acc)),
block(MakeRuntimeParams(staticModule, moduleSlot, staticModule.block)) {}
