#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGUIState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <juce_core/juce_core.h>

using namespace juce;

static std::string const 
Magic = "{84A1EBED-4BE5-47F2-8E53-13B965628974}";

static bool
ParseJson(std::string const& text, var& json)
{
  auto parsed = JSON::parse(text, json);
  DynamicObject* obj = json.getDynamicObject();
  if (!parsed.wasOk() || obj == nullptr)
    return false;
  return true;
}

static std::unordered_map<int, int>
MakeParamTagToIndex(
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
paramTagToIndex(MakeParamTagToIndex(params)) {}

std::string 
FBRuntimeTopo::SaveGUIStateToString(
  FBGUIState const& gui) const
{
  return SaveGUIStateToVar(gui).toString().toStdString();
}

std::string 
FBRuntimeTopo::SaveProcStateToString(
  FBProcStateContainer const& proc) const
{
  return SaveProcStateToVar(proc).toString().toStdString();
}

std::string 
FBRuntimeTopo::SaveEditStateToString(
  FBScalarStateContainer const& edit) const
{
  return SaveEditStateToVar(edit).toString().toStdString();
}

bool 
FBRuntimeTopo::LoadGUIStateFromString(
  std::string const& text, FBGUIState& gui) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadGUIStateFromVar(json, gui);
}

bool
FBRuntimeTopo::LoadProcStateFromString(
  std::string const& text, FBProcStateContainer& proc) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadProcStateFromVar(json, proc);
}

bool 
FBRuntimeTopo::LoadEditStateFromString(
  std::string const& text, FBScalarStateContainer& edit) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadEditStateFromVar(json, edit);
}

var
FBRuntimeTopo::SaveProcStateToVar( 
  FBProcStateContainer const& proc) const
{
  FBScalarStateContainer edit(*this);
  edit.CopyFrom(proc);
  return SaveEditStateToVar(edit);
}

var 
FBRuntimeTopo::SaveGUIStateToVar(
  FBGUIState const& gui) const
{
  auto result = new DynamicObject;
  result->setProperty("userScale", gui.userScale);
  return var(result);
}

bool 
FBRuntimeTopo::LoadProcStateFromVar(
  var const& json, FBProcStateContainer& proc) const
{
  FBScalarStateContainer edit(*this);
  if (!LoadEditStateFromVar(json, edit))
    return false;
  proc.InitProcessing(edit);
  return true;
}

bool 
FBRuntimeTopo::LoadGUIStateFromVar(
  var const& json, FBGUIState& gui) const
{
  DynamicObject* obj = json.getDynamicObject();
  if (obj->hasProperty("userScale"))
    gui.userScale = std::clamp(
      (float)obj->getProperty("userScale"), 
      static_.gui.minUserScale, static_.gui.maxUserScale);
  return true;
}

var
FBRuntimeTopo::SaveEditStateToVar(
  FBScalarStateContainer const& edit) const
{
  var state;
  for (int p = 0; p < params.size(); p++)
  {
    auto param = new DynamicObject;
    param->setProperty("id", String(params[p].id));
    param->setProperty("val", String(params[p].static_.NormalizedToText(true, *edit.Params()[p])));
    state.append(var(param));
  }

  auto result = new DynamicObject;
  result->setProperty("magic", String(Magic));
  result->setProperty("major", static_.version.major);
  result->setProperty("minor", static_.version.minor);
  result->setProperty("patch", static_.version.patch);
  result->setProperty("state", state);
  return var(result);
}

bool 
FBRuntimeTopo::LoadEditStateFromVar(
  var const& json, FBScalarStateContainer& edit) const
{
  DynamicObject* obj = json.getDynamicObject();
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

  for (int p = 0; p < edit.Params().size(); p++)
  {
    float defaultNormalized = 0.0f;
    if(params[p].static_.defaultText.size())
      defaultNormalized = params[p].static_.TextToNormalized(false, params[p].static_.defaultText).value();
    *edit.Params()[p] = defaultNormalized;
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
    if ((iter = paramTagToIndex.find(tag)) == paramTagToIndex.end())
      continue;

    auto const& topo = params[iter->second].static_;
    auto normalized = topo.TextToNormalized(true, val.toString().toStdString());
    if (!normalized)
      normalized = topo.TextToNormalized(false, topo.defaultText);
    *edit.Params()[iter->second] = normalized.value();
  }

  return true;
}