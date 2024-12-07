#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

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
  std::vector<FBRuntimeModule> result;
  for (int m = 0; m < topo.modules.size(); m++)
    for (int s = 0; s < topo.modules[m].slotCount; s++)
      result.push_back(FBRuntimeModule(topo.modules[m], runtimeIndex++, m, s));
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
  std::string const& from, FBProcStatePtrs& to) const
{
  void* scalar = static_.allocScalarState();
  auto ptrs = MakeScalarStatePtrs(scalar);
  bool result = LoadState(from, ptrs);
  if (result)
    to.InitFrom(ptrs);
  static_.freeScalarState(scalar);
  return result;
}

std::string
FBRuntimeTopo::SaveState(FBProcStatePtrs const& from) const
{
  void* scalar = static_.allocScalarState();
  auto ptrs = MakeScalarStatePtrs(scalar);
  ptrs.InitFrom(from);
  auto result = SaveState(ptrs);
  static_.freeScalarState(scalar);
  return result;
}

FBScalarStatePtrs
FBRuntimeTopo::MakeScalarStatePtrs(void* scalar) const
{
  FBScalarStatePtrs result = {};
  result.values.clear();
  for (int p = 0; p < params.size(); p++)
    result.values.push_back(params[p].static_.scalarAddr(
      params[p].staticModuleSlot, params[p].staticSlot, scalar));
  return result;
}

FBProcStatePtrs
FBRuntimeTopo::MakeProcStatePtrs(void* proc) const
{
  FBProcStatePtrs result = {};
  for (int p = 0; p < params.size(); p++)
  {
    bool isAcc = params[p].static_.acc;
    bool isVoice = static_.modules[params[p].staticModuleIndex].voice;
    result.allBlock.emplace_back();
    result.voiceAcc.emplace_back();
    result.globalAcc.emplace_back();
    result.voiceBlock.emplace_back();
    result.isVoice.push_back(isVoice);
    result.isAcc.push_back(params[p].static_.acc);

    if (!isAcc)
    {
      result.allBlock[p] = params[p].static_.allBlockAddr(
        params[p].staticModuleSlot, params[p].staticSlot, proc);
      if(isVoice)
        for (int v = 0; v < FB_MAX_VOICES; v++)
          result.voiceBlock[p].voice[v] = params[p].static_.voiceBlockAddr(
            v, params[p].staticModuleSlot, params[p].staticSlot, proc);
    }
    else if(!isVoice)
      result.globalAcc[p] = params[p].static_.globalAccAddr(
        params[p].staticModuleSlot, params[p].staticSlot, proc);
    else
      for (int v = 0; v < FB_MAX_VOICES; v++)
        result.voiceAcc[p] = params[p].static_.voiceAccAddr(
          v, params[p].staticModuleSlot, params[p].staticSlot, proc);
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
    auto param = new DynamicObject;
    param->setProperty("id", String(params[p].id));
    param->setProperty("val", String(params[p].static_.NormalizedToText(true, *from.values[p])));
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
FBRuntimeTopo::LoadState(std::string const& from, FBScalarStatePtrs& to) const
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

  for (int p = 0; p < to.values.size(); p++)
    *to.values[p] = static_cast<float>(params[p].static_.defaultNormalized);

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
    auto normalized = topo.TextToNormalizedOrDefault(val.toString().toStdString(), true);
    *to.values[iter->second] = static_cast<float>(normalized);
  }

  return true;
}