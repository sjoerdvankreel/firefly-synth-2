#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

#include <juce_core/juce_core.h>

static std::string const 
Magic = "{84A1EBED-4BE5-47F2-8E53-13B965628974}";

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
MakeRuntimeParams(
  std::vector<FBRuntimeModule> const& modules)
{
  std::vector<FBRuntimeParam> result;
  for (int m = 0; m < modules.size(); m++)
    for (int a = 0; a < modules[m].params.size(); a++)
      result.push_back(modules[m].params[a]);
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
params(MakeRuntimeParams(modules)),
tagToParam(MakeTagToParam(params)) {}

FBScalarStatePtrs
FBRuntimeTopo::MakeScalarStatePtrs(void* scalar) const
{
  FBScalarStatePtrs result = {};
  result.all.clear();
  for (int p = 0; p < params.size(); p++)
    result.all.push_back(params[p].static_.scalarAddr(
      params[p].moduleSlot, params[p].paramSlot, scalar));
  return result;
}

FBProcStatePtrs
FBRuntimeTopo::MakeProcStatePtrs(void* proc) const
{
  FBProcStatePtrs result = {};
  result.acc.clear();
  result.block.clear();
  result.isBlock.clear();
  for (int p = 0; p < params.size(); p++)
  {
    result.acc.emplace_back();
    result.block.emplace_back();
    result.isBlock.push_back(params[p].static_.block);
    if(params[p].static_.block)
      result.block[p] = params[p].static_.procBlockAddr(
        params[p].moduleSlot, params[p].paramSlot, proc);
    else
      result.acc[p] = params[p].static_.procAccAddr(
        params[p].moduleSlot, params[p].paramSlot, proc);
  }
  return result;
}

std::string 
FBRuntimeTopo::SaveState(FBScalarStatePtrs const& from) const
{
  using juce::var;
  using juce::JSON;
  using juce::String;
  using juce::DynamicObject;

  var state;
  for (int p = 0; p < params.size(); p++)
  {
    DynamicObject param;
    param.setProperty("id", String(params[p].id));
    param.setProperty("val", String(params[p].static_.NormalizedToText(true, *from.all[p])));
    state.append(var(&param));
  }

  DynamicObject result;
  result.setProperty("magic", String(Magic));
  result.setProperty("version_major", static_.version.major);
  result.setProperty("version_minor", static_.version.minor);
  result.setProperty("version_patch", static_.version.patch);
  result.setProperty("state", state);
  return JSON::toString(var(&result)).toStdString();
}

void 
FBRuntimeTopo::LoadState(std::string const& state, FBScalarStatePtrs& to) const
{

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