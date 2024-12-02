#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base/base/plug/FBPlugState.hpp>

#include <cstdint>

static std::map<int, int>
MakeTagToParam(
  std::vector<FBRuntimeParam> const& params)
{
  std::map<int, int> result;
  for (int p = 0; p < params.size(); p++)
    result[params[p].tag] = p;
  return result;
}

static std::vector<FBRuntimeParam>
MakeRuntimeAcc(
  std::vector<FBRuntimeModule> const& modules)
{
  std::vector<FBRuntimeParam> result;
  for (int m = 0; m < modules.size(); m++)
    for (int a = 0; a < modules[m].acc.size(); a++)
      result.push_back(modules[m].acc[a]);
  return result;
}

static std::vector<FBRuntimeParam>
MakeRuntimeBlock(
  std::vector<FBRuntimeModule> const& modules)
{
  std::vector<FBRuntimeParam> result;
  for (int m = 0; m < modules.size(); m++)
    for (int b = 0; b < modules[m].block.size(); b++)
      result.push_back(modules[m].block[b]);
  return result;
}

static std::string
MakeRuntimeName(
  std::string const& name, int slotCount, int slot)
{
  std::string result = name;
  if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}

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

static std::vector<FBRuntimeModule>
MakeRuntimeModules(FBStaticTopo const& topo)
{
  std::vector<FBRuntimeModule> result;
  for (int m = 0; m < topo.modules.size(); m++)
    for (int s = 0; s < topo.modules[m].slotCount; s++)
      result.push_back(FBRuntimeModule(topo.modules[m], s));
  return result;
}

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

static std::string
MakeRuntimeParamLongName(
  FBStaticModule const& module, int moduleSlot,
  FBStaticParam const& param, int paramSlot)
{
  auto moduleName = MakeRuntimeName(module.name, module.slotCount, moduleSlot);
  auto paramName = MakeRuntimeName(param.name, param.slotCount, paramSlot);
  return moduleName + " " + paramName;
}

void
FBRuntimeTopo::InitProcAddrs(FBProcStateAddrs& addrs) const
{
  addrs.param.clear();
  for (int a = 0; a < acc.size(); a++)
    addrs.param.push_back(acc[a].static_.procAddr(
      acc[a].moduleSlot, acc[a].paramSlot, addrs));
}

void 
FBRuntimeTopo::InitScalarAddrs(FBScalarStateAddrs& addrs) const
{
  addrs.acc.clear();
  for (int a = 0; a < acc.size(); a++)
    addrs.acc.push_back(acc[a].static_.scalarAddr(
      acc[a].moduleSlot, acc[a].paramSlot, addrs));

  addrs.block.clear();
  for (int b = 0; b < block.size(); b++)
    addrs.block.push_back(block[b].static_.scalarAddr(
      block[b].moduleSlot, block[b].paramSlot, addrs));
}

FBRuntimeTopo::
FBRuntimeTopo(FBStaticTopo const& topo) :
static_(topo),
modules(MakeRuntimeModules(topo)),
acc(MakeRuntimeAcc(modules)),
block(MakeRuntimeBlock(modules)),
tagToAcc(MakeTagToParam(acc)),
tagToBlock(MakeTagToParam(block)) {}

FBRuntimeParam::
FBRuntimeParam(
FBStaticModule const& module, int moduleSlot,
FBStaticParam const& param, int paramSlot) :
moduleSlot(moduleSlot),
paramSlot(paramSlot),
static_(param),
longName(MakeRuntimeParamLongName(module, moduleSlot, param, paramSlot)),
shortName(MakeRuntimeName(param.name, param.slotCount, paramSlot)),
id(MakeRuntimeParamId(module, moduleSlot, param, paramSlot)),
tag(MakeRuntimeHash(id)) {}

FBRuntimeModule::
FBRuntimeModule(
FBStaticModule const& module, int moduleSlot) :
name(MakeRuntimeName(module.name, module.slotCount, moduleSlot)),
acc(MakeRuntimeParams(module, moduleSlot, module.acc)),
block(MakeRuntimeParams(module, moduleSlot, module.block)) {}