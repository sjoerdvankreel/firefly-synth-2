#include <playground_base/base/topo/runtime/FBTopoDetail.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <playground_base/base/shared/FBLogging.hpp>
#include <playground_base/base/state/main/FBGUIStateContainer.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>

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

static std::map<FBTopoIndices, int>
MakeModuleTopoToRuntime(
  std::vector<FBRuntimeModule> const& modules)
{
  std::map<FBTopoIndices, int> result;
  for (int m = 0; m < modules.size(); m++)
    result[modules[m].topoIndices] = m;
  return result;
}

static std::vector<FBRuntimeModule>
MakeRuntimeModules(FBStaticTopo const& topo)
{
  int runtimeIndex = 0;
  int runtimeParamStart = 0;
  int runtimeGUIParamStart = 0;
  std::vector<FBRuntimeModule> result;
  for (int m = 0; m < topo.modules.size(); m++)
    for (int s = 0; s < topo.modules[m].slotCount; s++)
    {
      FBTopoIndices topoIndices;
      topoIndices.slot = s;
      topoIndices.index = m;
      auto module = FBRuntimeModule(topo.modules[m], topoIndices, runtimeIndex++, runtimeParamStart, runtimeGUIParamStart);
      result.push_back(module);
      runtimeParamStart += static_cast<int>(module.params.size());
      runtimeGUIParamStart += static_cast<int>(module.guiParams.size());
    }
  return result;
}

FBRuntimeTopo::
FBRuntimeTopo(FBStaticTopo const& topo) :
static_(topo),
modules(MakeRuntimeModules(topo)),
audio(FBRuntimeParamsTopo<FBRuntimeParam>(modules)),
gui(FBRuntimeParamsTopo<FBRuntimeGUIParam>(modules)),
moduleTopoToRuntime(MakeModuleTopoToRuntime(modules))
{
#ifndef NDEBUG
  std::set<std::string> allIds = {};
  std::set<std::string> moduleNames = {};
  for (int m = 0; m < topo.modules.size(); m++)
  {
    auto const& module = topo.modules[m];
    assert(allIds.insert(module.id).second);
    assert(moduleNames.insert(module.name).second);
    for (int p = 0; p < module.params.size(); p++)
    {
      auto const& param = topo.modules[m].params[p];
      assert(allIds.insert(param.id).second);
      if (param.type == FBParamType::List)
        for (int i = 0; i < param.List().items.size(); i++)
          assert(allIds.insert(param.List().items[i].id).second);
    }
  }
#endif
}

FBRuntimeModule const* 
FBRuntimeTopo::ModuleAtTopo(
  FBTopoIndices const& topoIndices) const
{
  return &modules[moduleTopoToRuntime.at(topoIndices)];
}

var
FBRuntimeTopo::SaveEditStateToVar(
  FBScalarStateContainer const& edit) const
{
  return SaveParamStateToVar(edit, audio.params);
}

var
FBRuntimeTopo::SaveGUIStateToVar(
  FBGUIStateContainer const& gui) const
{
  return SaveParamStateToVar(gui, this->gui.params);
}

bool
FBRuntimeTopo::LoadEditStateFromVar(
  var const& json, FBScalarStateContainer& edit) const
{
  return LoadParamStateFromVar(json, edit, audio);
}

bool
FBRuntimeTopo::LoadGUIStateFromVar(
  var const& json, FBGUIStateContainer& gui) const
{
  return LoadParamStateFromVar(json, gui, this->gui);
}

std::string 
FBRuntimeTopo::SaveGUIStateToString(
  FBGUIStateContainer const& gui) const
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
  FBScalarStateContainer const& edit, FBGUIStateContainer const& gui) const
{
  return JSON::toString(SaveEditAndGUIStateToVar(edit, gui)).toStdString();
}

var
FBRuntimeTopo::SaveProcStateToVar(
  FBProcStateContainer const& proc) const
{
  FBScalarStateContainer edit(*this);
  edit.CopyFrom(proc);
  return SaveEditStateToVar(edit);
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
FBRuntimeTopo::LoadGUIStateFromString(
  std::string const& text, FBGUIStateContainer& gui) const
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
  std::string const& text, 
  FBScalarStateContainer& edit, FBGUIStateContainer& gui) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadEditAndGUIStateFromVar(json, edit, gui);
}

void 
FBRuntimeTopo::LoadGUIStateFromStringWithDryRun(
  std::string const& text, FBGUIStateContainer& gui) const
{
  FBGUIStateContainer dryGUI(*this);
  if (LoadGUIStateFromString(text, dryGUI))
    gui.CopyFrom(dryGUI);
}

void 
FBRuntimeTopo::LoadProcStateFromStringWithDryRun(
  std::string const& text, FBProcStateContainer& proc) const
{
  FBScalarStateContainer dryEdit(*this);
  if (LoadEditStateFromString(text, dryEdit))
    proc.InitProcessing(dryEdit);
}

void 
FBRuntimeTopo::LoadEditStateFromStringWithDryRun(
  std::string const& text, FBScalarStateContainer& edit) const
{
  FBScalarStateContainer dryEdit(*this);
  if (LoadEditStateFromString(text, dryEdit))
    edit.CopyFrom(dryEdit);
}

void 
FBRuntimeTopo::LoadEditAndGUIStateFromStringWithDryRun(
  std::string const& text, FBScalarStateContainer& edit, FBGUIStateContainer& gui) const
{
  FBGUIStateContainer dryGUI(*this);
  FBScalarStateContainer dryEdit(*this);
  if (!LoadEditAndGUIStateFromString(text, dryEdit, dryGUI))
    return;
  gui.CopyFrom(dryGUI);
  edit.CopyFrom(dryEdit);
}

var 
FBRuntimeTopo::SaveEditAndGUIStateToVar(
  FBScalarStateContainer const& edit, FBGUIStateContainer const& gui) const
{
  auto guiState = SaveGUIStateToVar(gui);
  auto editState = SaveEditStateToVar(edit);
  auto result = new DynamicObject;
  result->setProperty("gui", guiState);
  result->setProperty("edit", editState);
  return var(result);
}

bool 
FBRuntimeTopo::LoadEditAndGUIStateFromVar(
  var const& json, FBScalarStateContainer& edit, FBGUIStateContainer& gui) const
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

template <class TContainer, class TParam>
var
FBRuntimeTopo::SaveParamStateToVar(
  TContainer const& container, std::vector<TParam> const& params) const
{
  var state;
  for (int p = 0; p < params.size(); p++)
  {
    auto param = new DynamicObject;
    param->setProperty("id", String(params[p].id));
    param->setProperty("val", String(params[p].static_.NonRealTime().NormalizedToText(true, *container.Params()[p])));
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

template <class TContainer, class TParamsTopo>
bool 
FBRuntimeTopo::LoadParamStateFromVar(
  var const& json, TContainer& container, TParamsTopo& params) const
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

  if (static_cast<int>(major) > static_.meta.version.major ||
    static_cast<int>(major) == static_.meta.version.major && static_cast<int>(minor) > static_.meta.version.minor ||
    static_cast<int>(minor) == static_.meta.version.minor && static_cast<int>(patch) > static_.meta.version.patch)
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

  for (int p = 0; p < container.Params().size(); p++)
  {
    double defaultNormalized = 0.0f;
    if(params.params[p].static_.defaultText.size())
      defaultNormalized = params.params[p].static_.NonRealTime().TextToNormalized(false, params.params[p].static_.defaultText).value();
    *container.Params()[p] = static_cast<float>(defaultNormalized);
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
    if ((iter = params.paramTagToIndex.find(tag)) == params.paramTagToIndex.end())
    {
      FB_LOG_WARN("Unknown plugin parameter.");
      continue;
    }

    auto const& topo = params.params[iter->second].static_;
    auto normalized = topo.NonRealTime().TextToNormalized(true, val.toString().toStdString());
    if (!normalized)
    {
      FB_LOG_WARN("Failed to parse plugin parameter value.");
      normalized = topo.NonRealTime().TextToNormalized(false, topo.defaultText);
    }
    *container.Params()[iter->second] = static_cast<float>(normalized.value());
  }

  return true;
}
