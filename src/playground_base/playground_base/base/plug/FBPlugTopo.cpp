#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base/base/plug/FBPlugState.hpp>
#include <juce_core/juce_core.h>

#include <cstdint>

using namespace juce;

static var
SaveParamState(std::vector<FBRuntimeParam> const& params)
{
  var result;
  for (int a = 0; a < acc.size(); a++)
  {
    DynamicObject param;
    param.setProperty("id", String(acc[a].id));
    param.setProperty("val", String(acc[a].static_.NormalizedToText(true, *state.acc[a])));
    accState.append(var(&param));
  }
}

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
FBRuntimeTopo::InitProcAddrs(FBProcStateAddrs& state) const
{
  state.param.clear();
  for (int a = 0; a < acc.size(); a++)
    state.param.push_back(acc[a].static_.procAddr(
      acc[a].moduleSlot, acc[a].paramSlot, state));
}

void 
FBRuntimeTopo::InitScalarAddrs(FBScalarStateAddrs& state) const
{
  state.acc.clear();
  for (int a = 0; a < acc.size(); a++)
    state.acc.push_back(acc[a].static_.scalarAddr(
      acc[a].moduleSlot, acc[a].paramSlot, state));

  state.block.clear();
  for (int b = 0; b < block.size(); b++)
    state.block.push_back(block[b].static_.scalarAddr(
      block[b].moduleSlot, block[b].paramSlot, state));
}

std::string
FBStaticParam::NormalizedToText(bool io, double normalized) const
{
  assert(valueCount != 1);
  if (valueCount == 0)
    return std::to_string(normalized);
  int discrete = NormalizedToDiscrete(normalized);
  if (list.size() != 0)
    return io ? list[discrete].id : list[discrete].text;
  if (valueCount == 2)
    return discrete == 0 ? "Off" : "On";
  return std::to_string(discrete);
}

std::string 
FBRuntimeTopo::SaveState(FBScalarStateAddrs const& state) const
{
  // TODO magic and version

  using juce::var;
  using juce::String;
  using juce::DynamicObject;

  DynamicObject result;

  var accState;
  for (int a = 0; a < acc.size(); a++)
  {
    DynamicObject param;
    param.setProperty("id", String(acc[a].id));
    param.setProperty("val", String(acc[a].static_.NormalizedToText(true, *state.acc[a])));
    accState.append(var(&param));
  }
  result.setProperty("acc", accState);
  
  var blockState;
  for (int b = 0; b < block.size(); b++)
  {
    DynamicObject param;
    param.setProperty("id", String(block[b].id));
    param.setProperty("val", String(block[b].static_.NormalizedToText(true, *state.block[b])));
    blockState.append(var(&param));
  }
  result.setProperty("block", blockState);


}

void 
FBRuntimeTopo::LoadState(
  std::string const& stored, FBScalarStateAddrs const& state) const
{

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