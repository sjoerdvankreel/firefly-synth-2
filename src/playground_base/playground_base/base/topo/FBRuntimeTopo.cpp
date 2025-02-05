#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <playground_base/base/shared/FBLogging.hpp>
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
  {
    FB_LOG_ERROR("Failed to parse json.");
    return false;
  }
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

static std::map<FBParamTopoIndices, int>
MakeParamTopoToRuntime(
  std::vector<FBRuntimeParam> const& params)
{
  std::map<FBParamTopoIndices, int> result;
  for (int p = 0; p < params.size(); p++)
    result[params[p].topoIndices] = p;
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
      FBTopoIndices topoIndices;
      topoIndices.slot = s;
      topoIndices.index = m;
      auto module = FBRuntimeModule(topo.modules[m], topoIndices, runtimeIndex++, runtimeParamStart);
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
paramTagToIndex(MakeParamTagToIndex(params)),
paramTopoToRuntime(MakeParamTopoToRuntime(params)) {}

FBRuntimeParam const*
FBRuntimeTopo::ParamAtTopo(
  FBParamTopoIndices const& topoIndices) const
{
  return &params[paramTopoToRuntime.at(topoIndices)];
}

FBRuntimeModule const* 
FBRuntimeTopo::ModuleAtParamIndex(int runtimeParamIndex) const
{
  return &modules[params[runtimeParamIndex].runtimeModuleIndex];
}

std::string 
FBRuntimeTopo::SaveGUIStateToString(
  FBGUIState const& gui) const
{
  return JSON::toString(SaveGUIStateToVar(gui)).toStdString();
}

std::string 
FBRuntimeTopo::SaveProcStateToString(
  FBProcStateContainer const& proc) const
{
  return JSON::toString(SaveProcStateToVar(proc)).toStdString();
}

std::string 
FBRuntimeTopo::SaveEditStateToString(
  FBScalarStateContainer const& edit) const
{
  return JSON::toString(SaveEditStateToVar(edit)).toStdString();
}

std::string 
FBRuntimeTopo::SaveEditAndGUIStateToString(
  FBScalarStateContainer const& edit, FBGUIState const& gui) const
{
  return JSON::toString(SaveEditAndGUIStateToVar(edit, gui)).toStdString();
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

bool 
FBRuntimeTopo::LoadEditAndGUIStateFromString(
  std::string const& text, FBScalarStateContainer& edit, FBGUIState& gui) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadEditAndGUIStateFromVar(json, edit, gui);
}

bool 
FBRuntimeTopo::LoadGUIStateFromStringWithDryRun(
  std::string const& text, FBGUIState& gui) const
{
  FBGUIState dryGUI = {};
  if (!LoadGUIStateFromString(text, dryGUI))
    return false;
  gui = dryGUI;
  return true;
}

bool 
FBRuntimeTopo::LoadProcStateFromStringWithDryRun(
  std::string const& text, FBProcStateContainer& proc) const
{
  FBScalarStateContainer dryEdit(*this);
  if (!LoadEditStateFromString(text, dryEdit))
    return false;
  proc.InitProcessing(dryEdit);
  return true;
}

bool 
FBRuntimeTopo::LoadEditStateFromStringWithDryRun(
  std::string const& text, FBScalarStateContainer& edit) const
{
  FBScalarStateContainer dryEdit(*this);
  if (!LoadEditStateFromString(text, dryEdit))
    return false;
  edit.CopyFrom(dryEdit);
  return true;
}

bool 
FBRuntimeTopo::LoadEditAndGUIStateFromStringWithDryRun(
  std::string const& text, FBScalarStateContainer& edit, FBGUIState& gui) const
{
  FBGUIState dryGUI = {};
  FBScalarStateContainer dryEdit(*this);
  if (!LoadEditAndGUIStateFromString(text, dryEdit, dryGUI))
    return false;
  gui = dryGUI;
  edit.CopyFrom(dryEdit);
  return true;
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

var 
FBRuntimeTopo::SaveEditAndGUIStateToVar(
  FBScalarStateContainer const& edit, FBGUIState const& gui) const
{
  auto guiState = SaveGUIStateToVar(gui);
  auto editState = SaveEditStateToVar(edit);
  auto result = new DynamicObject;
  result->setProperty("gui", guiState);
  result->setProperty("edit", editState);
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
  {
    var userScale = obj->getProperty("userScale");
    if(userScale.isDouble())
      gui.userScale = std::clamp(
        (float)(double)obj->getProperty("userScale"),
        static_.gui.minUserScale, static_.gui.maxUserScale);
  }
  return true;
}

bool 
FBRuntimeTopo::LoadEditAndGUIStateFromVar(
  var const& json, FBScalarStateContainer& edit, FBGUIState& gui) const
{
  DynamicObject* obj = json.getDynamicObject();
  if (obj->hasProperty("gui"))
    LoadGUIStateFromVar(obj->getProperty("gui"), gui);
  if (obj->hasProperty("edit"))
    return LoadEditStateFromVar(obj->getProperty("edit"), edit);
  FB_LOG_ERROR("Missing edit state.");
  FBScalarStateContainer editDefault(*this);
  edit.CopyFrom(editDefault);
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
    param->setProperty("val", String(params[p].static_.NormalizedToText(FBValueTextDisplay::IO, *edit.Params()[p])));
    state.append(var(param));
  }

  auto result = new DynamicObject;
  result->setProperty("magic", String(Magic));
  result->setProperty("id", String(static_.meta.id));
  result->setProperty("major", static_.meta.version.major);
  result->setProperty("minor", static_.meta.version.minor);
  result->setProperty("patch", static_.meta.version.patch);
  result->setProperty("state", state);
  return var(result);
}

bool 
FBRuntimeTopo::LoadEditStateFromVar(
  var const& json, FBScalarStateContainer& edit) const
{
  FB_LOG_ENTRY_EXIT();

  DynamicObject* obj = json.getDynamicObject();
  if (!obj->hasProperty("magic"))
  {
    FB_LOG_ERROR("Missing file magic.");
    return false;
  }
  var magic = obj->getProperty("magic");
  if (!magic.isString())
  {
    FB_LOG_ERROR("File magic is not a string.");
    return false;
  }
  if (magic.toString() != String(Magic))
  {
    FB_LOG_ERROR("File magic mismatch.");
    return false;
  }

  if (!obj->hasProperty("id"))
  {
    FB_LOG_ERROR("Missing plugin id.");
    return false;
  }
  var id = obj->getProperty("id");
  if (!id.isString())
  {
    FB_LOG_ERROR("Plugin id is not a string.");
    return false;
  }
  if (id.toString().toStdString() != static_.meta.id)
  {
    FB_LOG_ERROR("Plugin id mismatch.");
    return false;
  }

  if (!obj->hasProperty("major"))
  {
    FB_LOG_ERROR("Missing plugin major version.");
    return false;
  }
  var major = obj->getProperty("major");
  if (!major.isInt())
  {
    FB_LOG_ERROR("Plugin major version is not an int.");
    return false;
  }

  if (!obj->hasProperty("minor"))
  {
    FB_LOG_ERROR("Missing plugin minor version.");
    return false;
  }
  var minor = obj->getProperty("minor");
  if (!minor.isInt())
  {
    FB_LOG_ERROR("Plugin minor version is not an int.");
    return false;
  }

  if (!obj->hasProperty("patch"))
  {
    FB_LOG_ERROR("Missing plugin patch version.");
    return false;
  }
  var patch = obj->getProperty("patch");
  if (!patch.isInt())
  {
    FB_LOG_ERROR("Plugin patch version is not an int.");
    return false;
  }

  if ((int)major > static_.meta.version.major ||
    (int)major == static_.meta.version.major && (int)minor > static_.meta.version.minor ||
    (int)minor == static_.meta.version.minor && (int)patch > static_.meta.version.patch)
  {
    FB_LOG_ERROR("Stored plugin version is newer than current plugin version.");
    return false;
  }

  if (!obj->hasProperty("state"))
  {
    FB_LOG_ERROR("Missing plugin state.");
    return false;
  }
  var state = obj->getProperty("state");
  if (!state.isArray())
  {
    FB_LOG_ERROR("Plugin state is not an array.");
    return false;
  }

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
    {
      FB_LOG_ERROR("Plugin param state is missing id.");
      return false;
    }
    var id = param->getProperty("id");
    if (!id.isString())
    {
      FB_LOG_ERROR("Plugin param state id is not a string.");
      return false;
    }

    if (!param->hasProperty("val"))
    {
      FB_LOG_ERROR("Plugin param state is missing value.");
      return false;
    }
    var val = param->getProperty("val");
    if (!val.isString())
    {
      FB_LOG_ERROR("Plugin param state value is not a string.");
      return false;
    }

    std::unordered_map<int, int>::const_iterator iter;
    int tag = FBMakeStableHash(id.toString().toStdString());
    if ((iter = paramTagToIndex.find(tag)) == paramTagToIndex.end())
    {
      FB_LOG_WARN("Unknown plugin parameter.");
      continue;
    }

    auto const& topo = params[iter->second].static_;
    auto normalized = topo.TextToNormalized(true, val.toString().toStdString());
    if (!normalized)
    {
      FB_LOG_WARN("Failed to parse plugin parameter value.");
      normalized = topo.TextToNormalized(false, topo.defaultText);
    }
    *edit.Params()[iter->second] = normalized.value();
  }

  return true;
}