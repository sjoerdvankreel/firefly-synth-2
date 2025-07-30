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
  int runtimeCVOutputStart = 0;
  std::vector<FBRuntimeModule> result;
  for (int m = 0; m < topo.modules.size(); m++)
    for (int s = 0; s < topo.modules[m].slotCount; s++)
    {
      FBTopoIndices topoIndices;
      topoIndices.slot = s;
      topoIndices.index = m;
      auto module = FBRuntimeModule(topo, topo.modules[m], topoIndices, runtimeIndex++, runtimeParamStart, runtimeGUIParamStart, runtimeCVOutputStart);
      result.push_back(module);
      runtimeParamStart += static_cast<int>(module.params.size());
      runtimeGUIParamStart += static_cast<int>(module.guiParams.size());
      runtimeCVOutputStart += static_cast<int>(module.cvOutputs.size());
    }
  return result;
}

FBRuntimeTopo::
FBRuntimeTopo(std::unique_ptr<FBStaticTopo>&& topo_) :
static_(std::move(topo_)),
modules(MakeRuntimeModules(*static_)),
audio(FBRuntimeParamsTopo<FBRuntimeParam>(modules)),
gui(FBRuntimeParamsTopo<FBRuntimeGUIParam>(modules)),
moduleTopoToRuntime(MakeModuleTopoToRuntime(modules))
{
#ifndef NDEBUG
  std::set<std::string> allIds = {};
  std::set<std::string> moduleNames = {};
  for (int m = 0; m < static_->modules.size(); m++)
  {
    auto const& module = static_->modules[m];
    FB_ASSERT(allIds.insert(module.id).second);
    FB_ASSERT(moduleNames.insert(module.name).second);
    for (int p = 0; p < module.params.size(); p++)
    {
      auto const& param = static_->modules[m].params[p];
      FB_ASSERT(allIds.insert(param.id).second);
      if (param.type == FBParamType::List)
      {
        std::set<std::string> allListIds = {};
        for (int i = 0; i < param.List().items.size(); i++)
          FB_ASSERT(allListIds.insert(param.List().items[i].id).second);
      }
    }
  }
  std::set<FBTopoIndices> allProcessorOrder = {};
  for (int m = 0; m < static_->modules.size(); m++)
    for (int s = 0; s < static_->modules[m].slotCount; s++)
      allProcessorOrder.insert({ m, s });
  std::set<FBTopoIndices> plugProcessorOrder = {};
  for (int o = 0; o < static_->moduleProcessOrder.size(); o++)
    FB_ASSERT(plugProcessorOrder.insert(static_->moduleProcessOrder[o]).second);
  FB_ASSERT(allProcessorOrder.size() == plugProcessorOrder.size());
  for (auto const& e : allProcessorOrder)
    FB_ASSERT(plugProcessorOrder.contains(e));
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
  return LoadParamStateFromVar(false, json, editState, audio);
}

bool
FBRuntimeTopo::LoadGUIStateFromVar(
  var const& json, FBGUIStateContainer& guiState) const
{
  return LoadParamStateFromVar(true, json, guiState, this->gui);
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
    if (params[p].static_.IsOutput())
      continue;

    auto param = new DynamicObject;
    param->setProperty("tag", params[p].tag);
    param->setProperty("id", String(params[p].id));
    param->setProperty("index", p);
    param->setProperty("paramId", String(params[p].Static().id));
    param->setProperty("moduleId", String(params[p].staticModuleId));
    param->setProperty("paramSlot", params[p].topoIndices.param.slot);
    param->setProperty("moduleSlot", params[p].topoIndices.module.slot);
    param->setProperty("val", String(params[p].NormalizedToText(true, *container.Params()[p])));

    // store the longname without funky chars
    // i dont want to deal with charsets in stored-to-disk
    // it's just meta anyway, maybe useful to aid conversion debugging
    std::string longNameClean = "";
    std::string longName = params[p].longName;
    for (int i = 0; i < longName.size(); i++)
    {
      char c = longName[i];
      if('0' <= c && c <= '9' || 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == ' ' || c == '-' || c == '_')
        longNameClean.push_back(longName[i]);
      else
        longNameClean.push_back('?');
    }
    param->setProperty("name", String(longNameClean));

    state.append(var(param));
  }

  auto result = new DynamicObject;
  result->setProperty("magic", String(Magic));
  result->setProperty("id", String(static_->meta.id));
  result->setProperty("major", static_->meta.version.major);
  result->setProperty("minor", static_->meta.version.minor);
  result->setProperty("patch", static_->meta.version.patch);
  result->setProperty("state", state);
  return var(result);
}

template <class TContainer, class TParamsTopo>
bool 
FBRuntimeTopo::LoadParamStateFromVar(
  bool isGuiState, var const& json, TContainer& container, TParamsTopo& params) const
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
  if (id.toString().toStdString() != static_->meta.id)
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

  FBPlugVersion loadingVersion = {};
  loadingVersion.major = static_cast<int>(major);
  loadingVersion.minor = static_cast<int>(minor);
  loadingVersion.patch = static_cast<int>(patch);

  if (static_->meta.version < loadingVersion)
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
      defaultNormalized = params.params[p].TextToNormalized(false, defaultText).value();
    *container.Params()[p] = static_cast<float>(defaultNormalized);
  }

  auto converter = static_->deserializationConverterFactory(loadingVersion, this);
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
    int oldTag = FBMakeStableHash(id.toString().toStdString());
    if ((iter = params.paramTagToIndex.find(oldTag)) == params.paramTagToIndex.end())
    {
      FB_LOG_WARN("Unknown plugin parameter: '" + id.toString().toStdString() + "', trying to map old to new.");
      var oldModuleId = param->getProperty("moduleId");
      if (!oldModuleId.isString())
      {
        FB_LOG_ERROR("Old module id is not a string.");
        continue;
      }
      var oldModuleSlot = param->getProperty("moduleSlot");
      if (!oldModuleSlot.isInt())
      {
        FB_LOG_ERROR("Old module slot is not an int.");
        continue;
      }
      var oldParamId = param->getProperty("paramId");
      if (!oldParamId.isString())
      {
        FB_LOG_ERROR("Old param id is not a string.");
        continue;
      }
      var oldParamSlot = param->getProperty("paramSlot");
      if (!oldParamSlot.isInt())
      {
        FB_LOG_ERROR("Old param slot is not an int.");
        continue;
      }
      int newParamSlot = -1;
      int newModuleSlot = -1;
      std::string newParamId = {};
      std::string newModuleId = {};
      if (!converter->OnParamNotFound(
        isGuiState,
        oldModuleId.toString().toStdString(), static_cast<int>(oldModuleSlot), 
        oldParamId.toString().toStdString(), static_cast<int>(oldParamSlot),
        newModuleId, newModuleSlot, newParamId, newParamSlot))
      {
        FB_LOG_ERROR("Failed to map old to new plugin parameter.");
        continue;
      }
      std::string newId = FBMakeRuntimeId(newModuleId, newModuleSlot, newParamId, newParamSlot);
      int newTag = FBMakeStableHash(newId);
      if ((iter = params.paramTagToIndex.find(newTag)) == params.paramTagToIndex.end())
      {
        FB_LOG_ERROR("Mapped old to new plugin parameter, but new id does not exist.");
        continue;
      }
      FB_LOG_INFO("Mapped old to new parameter: '" + newId + "'.");
    }

    auto const& topo = params.params[iter->second];
    auto normalized = topo.TextToNormalized(true, val.toString().toStdString());
    if (!normalized)
    {
      FB_LOG_WARN("Failed to parse plugin parameter value.");
      normalized = topo.TextToNormalized(false, topo.GetDefaultText());
#ifndef NDEBUG
      if (!normalized)
      {
        auto defaultText = topo.GetDefaultText();
        topo.TextToNormalized(false, defaultText);
        FB_ASSERT(normalized);
      }
#endif
    }
    if(!topo.static_.IsOutput())
      *container.Params()[iter->second] = static_cast<float>(normalized.value());
  }

  FB_LOG_INFO("Start deserialization post-processing.");
  converter->PostProcess(isGuiState, container.Params());
  FB_LOG_INFO("End deserialization post-processing.");

  return true;
}