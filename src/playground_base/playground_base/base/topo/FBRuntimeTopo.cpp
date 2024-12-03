#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

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

static std::vector<FBRuntimeModule>
MakeRuntimeModules(FBStaticTopo const& topo)
{
  std::vector<FBRuntimeModule> result;
  for (int m = 0; m < topo.modules.size(); m++)
    for (int s = 0; s < topo.modules[m].slotCount; s++)
      result.push_back(FBRuntimeModule(topo.modules[m], s));
  return result;
}

FBRuntimeTopo::
FBRuntimeTopo(FBStaticTopo const& topo) :
static_(topo),
modules(MakeRuntimeModules(topo)),
acc(MakeRuntimeAcc(modules)),
block(MakeRuntimeBlock(modules)),
tagToAcc(MakeTagToParam(acc)),
tagToBlock(MakeTagToParam(block)) {}

void
FBRuntimeTopo::InitProcState(FBProcState& state) const
{
  state.dense.clear();
  for (int a = 0; a < acc.size(); a++)
    state.dense.push_back(acc[a].static_.procAddr(
      acc[a].moduleSlot, acc[a].paramSlot, state));
}

void 
FBRuntimeTopo::InitScalarState(FBScalarState& state) const
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

#if 0 // TODO



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

#endif