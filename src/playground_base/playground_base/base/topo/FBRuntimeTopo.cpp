#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <juce_core/juce_core.h>

static std::string const 
Magic = "{84A1EBED-4BE5-47F2-8E53-13B965628974}";

static std::unordered_map<int, int>
MakeTagToParam(
  std::vector<FBRuntimeParam> const& params)
{
  std::unordered_map<int, int> result;
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
  int runtimeIndex = 0;
  int runtimeParamStart = 0;
  std::vector<FBRuntimeModule> result;
  for (int m = 0; m < topo.modules.size(); m++)
    for (int s = 0; s < topo.modules[m].slotCount; s++)
    {
      auto module = FBRuntimeModule(topo.modules[m], runtimeIndex++, runtimeParamStart, m, s);
      result.push_back(module);
      runtimeParamStart += (int)module.params.size();
    }
  return result;
}

FBRuntimeTopo::
FBRuntimeTopo(FBStaticTopo const& topo) :
static_(topo),
modules(MakeRuntimeModules(topo)),
params(MakeRuntimeParams(modules)),
tagToParam(MakeTagToParam(params)) {}

bool
FBRuntimeTopo::LoadStateWithDryRun(
  std::string const& from, FBProcStateContainer& to) const
{
  FBScalarStateContainer scalar(*this);
  bool result = LoadState(from, scalar);
  if (result)
    to.InitProcessing(scalar);
  return result;
}

std::string
FBRuntimeTopo::SaveState(FBProcStateContainer const& from) const
{
  FBScalarStateContainer scalar(*this);
  scalar.CopyFrom(from);
  return SaveState(scalar);
}

std::string 
FBRuntimeTopo::SaveState(FBScalarStateContainer const& from) const
{
  using juce::var;
  using juce::JSON;
  using juce::String;
  using juce::DynamicObject;

  var state;
  for (int p = 0; p < params.size(); p++)
  {
    auto param = new DynamicObject;
    param->setProperty("id", String(params[p].id));
    param->setProperty("val", String(params[p].static_.NormalizedToText(true, *from.Params()[p])));
    state.append(var(param));
  }

  auto result = new DynamicObject;
  result->setProperty("magic", String(Magic));
  result->setProperty("major", static_.version.major);
  result->setProperty("minor", static_.version.minor);
  result->setProperty("patch", static_.version.patch);
  result->setProperty("state", state);
  return JSON::toString(var(result)).toStdString();
}

bool 
FBRuntimeTopo::LoadState(std::string const& from, FBScalarStateContainer& to) const
{
  using juce::var;
  using juce::JSON;
  using juce::String;
  using juce::DynamicObject;
  
  var json;
  auto parsed = JSON::parse(from, json);
  DynamicObject* obj = json.getDynamicObject();
  if (!parsed.wasOk() || obj == nullptr)
    return false;

  if (!obj->hasProperty("magic"))
    return false;
  var magic = obj->getProperty("magic");
  if (!magic.isString())
    return false;
  if (magic.toString() != String(Magic))
    return false;

  if (!obj->hasProperty("major"))
    return false;
  var major = obj->getProperty("major");
  if (!major.isInt())
    return false;
  if (!obj->hasProperty("minor"))
    return false;
  var minor = obj->getProperty("minor");
  if (!minor.isInt())
    return false;
  if (!obj->hasProperty("patch"))
    return false;
  var patch = obj->getProperty("patch");
  if (!patch.isInt())
    return false;

  if ((int)major > static_.version.major)
    return false;
  if ((int)major == static_.version.major && (int)minor > static_.version.minor)
    return false;
  if ((int)minor == static_.version.minor && (int)patch > static_.version.patch)
    return false;

  if (!obj->hasProperty("state"))
    return false;
  var state = obj->getProperty("state");
  if (!state.isArray())
    return false;

  for (int p = 0; p < to.Params().size(); p++)
  {
    float defaultNormalized = 0.0f;
    if(params[p].static_.defaultText.size())
      defaultNormalized = params[p].static_.TextToNormalized(false, params[p].static_.defaultText).value();
    *to.Params()[p] = defaultNormalized;
  }

  for (int sp = 0; sp < state.size(); sp++)
  {
    DynamicObject* param = state[sp].getDynamicObject();
    
    if (!param->hasProperty("id"))
      return false;
    var id = param->getProperty("id");
    if (!id.isString())
      return false;

    if (!param->hasProperty("val"))
      return false;
    var val = param->getProperty("val");
    if (!val.isString())
      return false;

    std::unordered_map<int, int>::const_iterator iter;
    int tag = FBMakeStableHash(id.toString().toStdString());
    if ((iter = tagToParam.find(tag)) == tagToParam.end())
      continue;

    auto const& topo = params[iter->second].static_;
    auto normalized = topo.TextToNormalized(true, val.toString().toStdString());
    if (!normalized)
      normalized = topo.TextToNormalized(false, topo.defaultText);
    *to.Params()[iter->second] = static_cast<float>(normalized.value());
  }

  return true;
}