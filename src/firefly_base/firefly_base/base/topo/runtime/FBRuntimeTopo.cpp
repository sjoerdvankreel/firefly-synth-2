#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

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
    FB_ASSERT(allIds.insert(module.id).second);
    FB_ASSERT(moduleNames.insert(module.name).second);
    for (int p = 0; p < module.params.size(); p++)
    {
      auto const& param = topo.modules[m].params[p];
      FB_ASSERT(allIds.insert(param.id).second);
      if (param.type == FBParamType::List)
        for (int i = 0; i < param.List().items.size(); i++)
          FB_ASSERT(allIds.insert(param.List().items[i].id).second);
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
  FBScalarStateContainer const& editState) const
{
  return SaveParamStateToVar(editState, audio.params);
}

var
FBRuntimeTopo::SaveGUIStateToVar(
  FBGUIStateContainer const& guiState) const
{
  return SaveParamStateToVar(guiState, this->gui.params);
}

bool
FBRuntimeTopo::LoadEditStateFromVar(
  var const& json, FBScalarStateContainer& editState) const
{
  return LoadParamStateFromVar(json, editState, audio);
}

bool
FBRuntimeTopo::LoadGUIStateFromVar(
  var const& json, FBGUIStateContainer& guiState) const
{
  return LoadParamStateFromVar(json, guiState, this->gui);
}

std::string 
FBRuntimeTopo::SaveGUIStateToString(
  FBGUIStateContainer const& guiState) const
{
  return JSON::toString(SaveGUIStateToVar(guiState)).toStdString();
}

std::string 
FBRuntimeTopo::SaveProcStateToString(
  FBProcStateContainer const& procState) const
{
  return JSON::toString(SaveProcStateToVar(procState)).toStdString();
}

std::string 
FBRuntimeTopo::SaveEditStateToString(
  FBScalarStateContainer const& editState) const
{
  return JSON::toString(SaveEditStateToVar(editState)).toStdString();
}

std::string 
FBRuntimeTopo::SaveEditAndGUIStateToString(
  FBScalarStateContainer const& editState, FBGUIStateContainer const& guiState) const
{
  return JSON::toString(SaveEditAndGUIStateToVar(editState, guiState)).toStdString();
}

var
FBRuntimeTopo::SaveProcStateToVar(
  FBProcStateContainer const& procState) const
{
  FBScalarStateContainer editState(*this);
  editState.CopyFrom(procState);
  return SaveEditStateToVar(editState);
}

bool
FBRuntimeTopo::LoadProcStateFromVar(
  var const& json, FBProcStateContainer& procState) const
{
  FBScalarStateContainer editState(*this);
  if (!LoadEditStateFromVar(json, editState))
    return false;
  procState.InitProcessing(editState);
  return true;
}

bool 
FBRuntimeTopo::LoadGUIStateFromString(
  std::string const& text, FBGUIStateContainer& guiState) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadGUIStateFromVar(json, guiState);
}

bool
FBRuntimeTopo::LoadProcStateFromString(
  std::string const& text, FBProcStateContainer& procState) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadProcStateFromVar(json, procState);
}

bool 
FBRuntimeTopo::LoadEditStateFromString(
  std::string const& text, FBScalarStateContainer& editState) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadEditStateFromVar(json, editState);
}

bool 
FBRuntimeTopo::LoadEditAndGUIStateFromString(
  std::string const& text, 
  FBScalarStateContainer& editState, FBGUIStateContainer& guiState) const
{
  var json;
  if (!ParseJson(text, json))
    return false;
  return LoadEditAndGUIStateFromVar(json, editState, guiState);
}

void 
FBRuntimeTopo::LoadGUIStateFromStringWithDryRun(
  std::string const& text, FBGUIStateContainer& guiState) const
{
  FBGUIStateContainer dryGUIState(*this);
  if (LoadGUIStateFromString(text, dryGUIState))
    guiState.CopyFrom(dryGUIState);
}

void 
FBRuntimeTopo::LoadProcStateFromStringWithDryRun(
  std::string const& text, FBProcStateContainer& procState) const
{
  FBScalarStateContainer dryEditState(*this);
  if (LoadEditStateFromString(text, dryEditState))
    procState.InitProcessing(dryEditState);
}

void 
FBRuntimeTopo::LoadEditStateFromStringWithDryRun(
  std::string const& text, FBScalarStateContainer& editState) const
{
  FBScalarStateContainer dryEditState(*this);
  if (LoadEditStateFromString(text, dryEditState))
    editState.CopyFrom(dryEditState);
}

void 
FBRuntimeTopo::LoadEditAndGUIStateFromStringWithDryRun(
  std::string const& text, FBScalarStateContainer& editState, FBGUIStateContainer& guiState) const
{
  FBGUIStateContainer dryGUIState(*this);
  FBScalarStateContainer dryEditState(*this);
  if (!LoadEditAndGUIStateFromString(text, dryEditState, dryGUIState))
    return;
  guiState.CopyFrom(dryGUIState);
  editState.CopyFrom(dryEditState);
}

var 
FBRuntimeTopo::SaveEditAndGUIStateToVar(
  FBScalarStateContainer const& editState, FBGUIStateContainer const& guiState) const
{
  auto guiStateVar = SaveGUIStateToVar(guiState);
  auto editStateVar = SaveEditStateToVar(editState);
  auto result = new DynamicObject;
  result->setProperty("gui", guiStateVar);
  result->setProperty("edit", editStateVar);
  return var(result);
}

bool 
FBRuntimeTopo::LoadEditAndGUIStateFromVar(
  var const& json, FBScalarStateContainer& editState, FBGUIStateContainer& guiState) const
{
  DynamicObject* obj = json.getDynamicObject();
  if (obj->hasProperty("gui"))
    LoadGUIStateFromVar(obj->getProperty("gui"), guiState);
  if (obj->hasProperty("edit"))
    return LoadEditStateFromVar(obj->getProperty("edit"), editState);
  FB_LOG_ERROR("Missing edit state.");
  FBScalarStateContainer editDefaultState(*this);
  editState.CopyFrom(editDefaultState);
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
    auto defaultText = params.params[p].GetDefaultText();
    if(defaultText.size())
      defaultNormalized = params.params[p].static_.NonRealTime().TextToNormalized(false, defaultText).value();
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
    id = param->getProperty("id");
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

    auto const& topo = params.params[iter->second];
    auto normalized = topo.static_.NonRealTime().TextToNormalized(true, val.toString().toStdString());
    if (!normalized)
    {
      FB_LOG_WARN("Failed to parse plugin parameter value.");
      normalized = topo.static_.NonRealTime().TextToNormalized(false, topo.GetDefaultText());
    }
    *container.Params()[iter->second] = static_cast<float>(normalized.value());
  }

  return true;
}
