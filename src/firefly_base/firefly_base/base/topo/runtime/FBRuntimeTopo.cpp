#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <firefly_base/base/shared/FBHostContext.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <juce_core/juce_core.h>
#include <sstream>

using namespace juce;

static int const
FileFormatVersion = 2;
static std::string const
Magic = "{84A1EBED-4BE5-47F2-8E53-13B965628974}";

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

std::string
FBRuntimeTopo::PrintParamList() const
{
  std::ostringstream result = {};
  for (int i = 0; i < audio.params.size(); i++)
  {
    auto const& param = audio.params[i];
    result << "Index: " << i << "\n";
    result << "\tTag: " << param.tag << "\n";
    result << "\tName: " << param.longName << "\n";
    result << "\tId: " << param.id << "\n";
  }
  return result.str();
}

std::string 
FBRuntimeTopo::SaveStateToString(
  FBHostContext const& hostContext, 
  FBScalarStateContainer const& audioParams,
  bool patchOnly) const
{
  return JSON::toString(SaveStateToVar(hostContext, audioParams, patchOnly)).toStdString();
}

bool 
FBRuntimeTopo::LoadStateFromString(
  std::string const& text, 
  FBHostContext& hostContext, 
  FBScalarStateContainer& audioParams, 
  bool patchOnly) const
{
  var json;
  if (!FBParseJson(text, json))
    return false;
  return LoadStateFromVar(json, hostContext, audioParams, patchOnly);
}

juce::var 
FBRuntimeTopo::SaveStateToVar(
  FBHostContext const& hostContext,
  FBScalarStateContainer const& audioParams, 
  bool patchOnly) const
{
  auto result = new DynamicObject;
  result->setProperty("magic", String(Magic));
  result->setProperty("fileFormatVersion", FileFormatVersion);
  result->setProperty("pluginId", String(static_->meta.id));
  result->setProperty("pluginVersionMajor", static_->meta.version.major);
  result->setProperty("pluginVersionMinor", static_->meta.version.minor);
  result->setProperty("pluginVersionPatch", static_->meta.version.patch);
  result->setProperty("patchName", String(hostContext.GUIState()->PatchName()));
  if (!patchOnly)
  {
    result->setProperty("themeName", String(hostContext.GUIState()->ThemeName()));
    result->setProperty("instanceName", String(hostContext.GUIState()->InstanceName()));
  }
  result->setProperty("guiAudioClips", SaveAudioClipsToVar(hostContext.GetGUIAudioClips()));
  result->setProperty("dspAudioClips", SaveAudioClipsToVar(hostContext.PullDSPAudioClipsToGUI()));
  result->setProperty("paramNameOverrides", SaveParamNameOverridesToVar(hostContext.GetAudioParamNameOverrides()));
  if (!patchOnly)
    result->setProperty("guiParams", SaveParamStateToVar(*hostContext.GUIState(), this->gui.params, patchOnly));
  result->setProperty("audioParams", SaveParamStateToVar(audioParams, this->audio.params, patchOnly));
  return var(result);
}

bool 
FBRuntimeTopo::LoadStateFromVar(
  juce::var const& json,
  FBHostContext& hostContext,
  FBScalarStateContainer& audioParams, bool patchOnly) const
{
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

  if (!obj->hasProperty("pluginId"))
  {
    FB_LOG_ERROR("Missing plugin id.");
    return false;
  }
  var pluginId = obj->getProperty("pluginId");
  if (!pluginId.isString())
  {
    FB_LOG_ERROR("Plugin id is not a string.");
    return false;
  }
  auto allowedIdIter = std::find(
    static_->meta.allowLoadFromIds.begin(),
    static_->meta.allowLoadFromIds.end(),
    pluginId.toString().toStdString());
  if (allowedIdIter == static_->meta.allowLoadFromIds.end())
  {
    FB_LOG_ERROR("Plugin id mismatch.");
    return false;
  }

  if (!obj->hasProperty("pluginVersionMajor"))
  {
    FB_LOG_ERROR("Missing plugin major version.");
    return false;
  }
  var pluginVersionMajor = obj->getProperty("pluginVersionMajor");
  if (!pluginVersionMajor.isInt())
  {
    FB_LOG_ERROR("Plugin major version is not an int.");
    return false;
  }

  if (!obj->hasProperty("pluginVersionMinor"))
  {
    FB_LOG_ERROR("Missing plugin minor version.");
    return false;
  }
  var pluginVersionMinor = obj->getProperty("pluginVersionMinor");
  if (!pluginVersionMinor.isInt())
  {
    FB_LOG_ERROR("Plugin minor version is not an int.");
    return false;
  }

  if (!obj->hasProperty("pluginVersionPatch"))
  {
    FB_LOG_ERROR("Missing plugin patch version.");
    return false;
  }
  var pluginVersionPatch = obj->getProperty("pluginVersionPatch");
  if (!pluginVersionPatch.isInt())
  {
    FB_LOG_ERROR("Plugin patch version is not an int.");
    return false;
  }

  FBPlugVersion loadingVersion = {};
  loadingVersion.major = static_cast<int>(pluginVersionMajor);
  loadingVersion.minor = static_cast<int>(pluginVersionMinor);
  loadingVersion.patch = static_cast<int>(pluginVersionPatch);

  if (static_->meta.version < loadingVersion)
  {
    FB_LOG_ERROR("Stored plugin version is newer than current plugin version.");
    return false;
  }

  if (!obj->hasProperty("patchName"))
  {
    FB_LOG_ERROR("Missing plugin patch name.");
    return false;
  }
  var patchNameVar = obj->getProperty("patchName");
  if (!patchNameVar.isString())
  {
    FB_LOG_ERROR("Plugin patch name is not a string.");
    return false;
  }
  std::string patchName = patchNameVar.toString().toStdString();

  std::string themeName;
  if (!patchOnly)
  {
    if (!obj->hasProperty("themeName"))
    {
      FB_LOG_ERROR("Missing plugin theme name.");
      return false;
    }
    var themeNameVar = obj->getProperty("themeName");
    if (!themeNameVar.isString())
    {
      FB_LOG_ERROR("Plugin theme name is not a string.");
      return false;
    }
    themeName = themeNameVar.toString().toStdString();
  }

  std::string instanceName;
  if (!patchOnly)
  {
    if (!obj->hasProperty("instanceName"))
    {
      FB_LOG_ERROR("Missing plugin instance name.");
      return false;
    }
    var instanceNameVar = obj->getProperty("instanceName");
    if (!instanceNameVar.isString())
    {
      FB_LOG_ERROR("Plugin instance name is not a string.");
      return false;
    }
    instanceName = instanceNameVar.toString().toStdString();
  }

  std::map<std::string, FBAudioClip> guiAudioClips;
  if (!obj->hasProperty("guiAudioClips"))
  {
    FB_LOG_ERROR("Missing plugin gui audio clips.");
    return false;
  }
  var guiAudioClipsVar = obj->getProperty("guiAudioClips");
  if (!guiAudioClipsVar.isObject())
  {
    FB_LOG_ERROR("Plugin gui audio clips is not an object.");
    return false;
  }
  if (!LoadAudioClipsFromVar(guiAudioClipsVar, guiAudioClips))
  {
    FB_LOG_ERROR("Failed to load plugin gui audio clips.");
    return false;
  }

  std::map<std::string, FBAudioClip> dspAudioClips;
  if (!obj->hasProperty("dspAudioClips"))
  {
    FB_LOG_ERROR("Missing plugin dsp audio clips.");
    return false;
  }
  var dspAudioClipsVar = obj->getProperty("dspAudioClips");
  if (!dspAudioClipsVar.isObject())
  {
    FB_LOG_ERROR("Plugin dsp audio clips is not an object.");
    return false;
  }
  if (!LoadAudioClipsFromVar(dspAudioClipsVar, dspAudioClips))
  {
    FB_LOG_ERROR("Failed to load plugin dsp audio clips.");
    return false;
  }

  std::map<int, std::string> paramNameOverrides;
  if (!obj->hasProperty("paramNameOverrides"))
  {
    FB_LOG_ERROR("Missing plugin param name overrides.");
    return false;
  }
  var paramNameOverridesVar = obj->getProperty("paramNameOverrides");
  if (!paramNameOverridesVar.isObject())
  {
    FB_LOG_ERROR("Plugin param name overrides is not an object.");
    return false;
  }
  if (!LoadParamNameOverridesFromVar(paramNameOverridesVar, paramNameOverrides))
  {
    FB_LOG_ERROR("Failed to load plugin param name overrides.");
    return false;
  }

  FBGUIStateContainer loadGuiParams(*this);
  if (!patchOnly)
  {
    if (!obj->hasProperty("guiParams"))
    {
      FB_LOG_ERROR("Missing plugin gui params.");
      return false;
    }
    var guiParamsVar = obj->getProperty("guiParams");
    if (!guiParamsVar.isArray())
    {
      FB_LOG_ERROR("Plugin gui params is not an array.");
      return false;
    }
    if (!LoadParamStateFromVar(guiParamsVar, loadingVersion, true, loadGuiParams, this->gui, false))
    {
      FB_LOG_ERROR("Failed to load plugin gui params.");
      return false;
    }
  }

  FBScalarStateContainer loadAudioParams(*this);
  if (!obj->hasProperty("audioParams"))
  {
    FB_LOG_ERROR("Missing plugin audio params.");
    return false;
  }
  var audioParamsVar = obj->getProperty("audioParams");
  if (!audioParamsVar.isArray())
  {
    FB_LOG_ERROR("Plugin audio params is not an array.");
    return false;
  }
  if (!LoadParamStateFromVar(audioParamsVar, loadingVersion, false, loadAudioParams, this->audio, patchOnly))
  {
    FB_LOG_ERROR("Failed to load plugin audio params.");
    return false;
  }

  hostContext.SetPatchName(patchName);
  if (!patchOnly)
  {
    hostContext.SetThemeName(themeName);
    hostContext.SetInstanceName(instanceName);
  }

  hostContext.SetGUIAudioClips(guiAudioClips);
  hostContext.PushDSPAudioClipsToDSP(dspAudioClips);
  hostContext.SetAudioParamNameOverrides(paramNameOverrides);

  if (!patchOnly)
    hostContext.GUIState()->CopyFrom(loadGuiParams);

  audioParams.CopyFrom(this, loadAudioParams, patchOnly);
  return true;
}

var
FBRuntimeTopo::SaveParamNameOverridesToVar(std::map<int, std::string> const& paramNameOverrides) const
{
  DynamicObject* result = new DynamicObject;
  for(auto const& kv: paramNameOverrides)
  {
    auto tag = String(std::to_string(audio.params[kv.first].tag));
    result->setProperty(tag, Base64::toBase64(String(kv.second)));
  }
  return var(result);
}

bool
FBRuntimeTopo::LoadParamNameOverridesFromVar(var const& json, std::map<int, std::string>& paramNameOverrides) const
{
  paramNameOverrides.clear();
  auto* obj = json.getDynamicObject();
  if (obj == nullptr)
    return false;

  for (auto const& prop : obj->getProperties())
  {
    char* unused = nullptr;
    auto tagString = prop.name.toString().toStdString();
    int tag = std::strtol(tagString.c_str(), &unused, 10);
    auto iter = audio.paramTagToIndex.find(tag);
    if (iter != audio.paramTagToIndex.end())
    {
      auto encoded = obj->getProperty(prop.name);
      if (encoded.isString())
      {
        juce::MemoryOutputStream ostream;
        if (Base64::convertFromBase64(ostream, encoded.toString()))
          paramNameOverrides[iter->second] = ostream.toString().toStdString();
      }
    }
  }
  return true;
}

var
FBRuntimeTopo::SaveAudioClipsToVar(std::map<std::string, FBAudioClip> const& audioClips) const
{
  DynamicObject* result = new DynamicObject;
  for (auto const& kv : audioClips)
  {
    DynamicObject* clip = new DynamicObject;
    clip->setProperty("name", String(kv.second.name));
    clip->setProperty("channels", (int)kv.second.channels);
    clip->setProperty("sampleRate", (int)kv.second.sampleRate);
    clip->setProperty("originalSampleRate", (int)kv.second.originalSampleRate);
    clip->setProperty("data", Base64::toBase64(kv.second.data.data(), kv.second.data.size() * sizeof(kv.second.data[0])));
    result->setProperty(String(kv.first), var(clip));
  }
  return var(result);
}

bool
FBRuntimeTopo::LoadAudioClipsFromVar(juce::var const& json, std::map<std::string, FBAudioClip>& audioClips) const
{
  audioClips.clear();
  auto* obj = json.getDynamicObject();
  if (obj == nullptr)
    return false;

  for (auto const& prop : obj->getProperties())
  {
    FBAudioClip clip;
    var const& clipJson = obj->getProperty(prop.name);
    if (!clipJson.hasProperty("name") || !clipJson["name"].isString())
      return false;
    clip.name = clipJson["name"].toString().toStdString();
    if (!clipJson.hasProperty("channels") || !clipJson["channels"].isInt())
      return false;
    clip.channels = (int)clipJson["channels"];
    if (!clipJson.hasProperty("sampleRate") || !clipJson["sampleRate"].isInt())
      return false;
    clip.sampleRate = (int)clipJson["sampleRate"];
    if (!clipJson.hasProperty("originalSampleRate") || !clipJson["originalSampleRate"].isInt())
      return false;
    clip.originalSampleRate = (int)clipJson["originalSampleRate"];
    if (!clipJson.hasProperty("data") || !clipJson["data"].isString())
      return false;
    MemoryOutputStream stream;
    if (!Base64::convertFromBase64(stream, clipJson["data"].toString()))
      return false;
    if (stream.getDataSize() % (sizeof(float) * clip.channels) != 0)
      return false;
    clip.data = std::vector<float>((float*)stream.getData(), (float*)((char*)stream.getData() + stream.getDataSize()));
    audioClips[prop.name.toString().toStdString()] = FBAudioClip(clip);
  }
  return true;
}

template <class TContainer, class TParam>
var
FBRuntimeTopo::SaveParamStateToVar(
  TContainer const& container, std::vector<TParam> const& params, bool patchOnly) const
{
  var state;
  for (int p = 0; p < params.size(); p++)
  {
    if (params[p].static_.IsOutput())
      continue;
    if (params[p].static_.IsFakeParam())
      continue;
    if (!params[p].static_.StoreInPatch() && patchOnly)
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
      if ('0' <= c && c <= '9' || 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == ' ' || c == '-' || c == '_')
        longNameClean.push_back(longName[i]);
      else
        longNameClean.push_back('?');
    }
    param->setProperty("name", String(longNameClean));
    state.append(var(param));
  }
  return state;
}

template <class TContainer, class TParamsTopo>
bool
FBRuntimeTopo::LoadParamStateFromVar(
  juce::var const& json, FBPlugVersion const& loadingVersion,
  bool isGuiState, TContainer& container, TParamsTopo& params, bool patchOnly) const
{
  FB_LOG_ENTRY_EXIT();
  for (int p = 0; p < container.Params().size(); p++)
  {
    double defaultNormalized = 0.0f;
    auto defaultText = params.params[p].GetDefaultText();
    if (defaultText.size())
      defaultNormalized = params.params[p].TextToNormalized(false, defaultText).value();

    if (!params.params[p].static_.IsOutput() &&
      !params.params[p].static_.IsFakeParam() &&
      (!patchOnly || params.params[p].static_.StoreInPatch()))
      *container.Params()[p] = static_cast<float>(defaultNormalized);
  }

  std::vector<FBOldParamInfo> oldParamInfos = {};
  auto converter = static_->deserializationConverterFactory(loadingVersion, this);
  for (int sp = 0; sp < json.size(); sp++)
  {
    DynamicObject* param = json[sp].getDynamicObject();

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

    if (!param->hasProperty("name"))
    {
      FB_LOG_ERROR("Plugin param state is missing name.");
      return false;
    }
    var name = param->getProperty("name");
    if (!name.isString())
    {
      FB_LOG_ERROR("Plugin param state name is not a string.");
      return false;
    }

    if (!param->hasProperty("moduleId"))
    {
      FB_LOG_ERROR("Plugin param state is missing module id.");
      return false;
    }
    var oldModuleId = param->getProperty("moduleId");
    if (!oldModuleId.isString())
    {
      FB_LOG_ERROR("Plugin param state module id is not a string.");
      return false;
    }

    if (!param->hasProperty("moduleSlot"))
    {
      FB_LOG_ERROR("Plugin param state is missing module slot.");
      return false;
    }
    var oldModuleSlot = param->getProperty("moduleSlot");
    if (!oldModuleSlot.isInt())
    {
      FB_LOG_ERROR("Plugin param state module slot is not an int.");
      return false;
    }

    if (!param->hasProperty("paramId"))
    {
      FB_LOG_ERROR("Plugin param state is missing param id.");
      return false;
    }
    var oldParamId = param->getProperty("paramId");
    if (!oldParamId.isString())
    {
      FB_LOG_ERROR("Plugin param state param id is not a string.");
      return false;
    }

    if (!param->hasProperty("paramSlot"))
    {
      FB_LOG_ERROR("Plugin param state is missing param slot.");
      return false;
    }
    var oldParamSlot = param->getProperty("paramSlot");
    if (!oldParamSlot.isInt())
    {
      FB_LOG_ERROR("Plugin param state param slot is not an int.");
      return false;
    }

    int oldTag = FBMakeStableHash(id.toString().toStdString());
    FBOldParamInfo oldParamInfo = {};
    oldParamInfo.index = sp;
    oldParamInfo.tag = oldTag;
    oldParamInfo.paramSlot = oldParamSlot;
    oldParamInfo.moduleSlot = oldModuleSlot;
    oldParamInfo.id = id.toString().toStdString();
    oldParamInfo.name = name.toString().toStdString();
    oldParamInfo.value = val.toString().toStdString();
    oldParamInfo.paramId = oldParamId.toString().toStdString();
    oldParamInfo.moduleId = oldModuleId.toString().toStdString();
    oldParamInfos.push_back(oldParamInfo);

    std::unordered_map<int, int>::const_iterator iter;
    if ((iter = params.paramTagToIndex.find(oldTag)) == params.paramTagToIndex.end())
    {
      FB_LOG_WARN("Unknown plugin parameter: '" + id.toString().toStdString() + "', trying to map old to new.");
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
      int paramSlot = topo.topoIndices.param.slot;
      int moduleSlot = topo.topoIndices.module.slot;
      std::string paramId = topo.Static().id;
      std::string moduleId = static_->modules[modules[topo.runtimeModuleIndex].topoIndices.index].id;

      FB_LOG_INFO(
        "Invalid param value found for " +
        moduleId + ", param " +
        paramId + ": " +
        val.toString().toStdString());

      if (topo.Static().type == FBParamType::List)
      {
        FB_LOG_INFO("Trying to map old to new list-valued parameter value.");
        std::string newParamValue = {};
        if (converter->OnParamListItemNotFound(isGuiState,
          moduleId, moduleSlot, paramId, paramSlot,
          val.toString().toStdString(), newParamValue))
        {
          normalized = topo.TextToNormalized(true, newParamValue);
          FB_LOG_INFO("Mapped old to new list-valued parameter value: " + val.toString().toStdString() + " to " + newParamValue + ".");
        }
        else
        {
          FB_LOG_WARN("Failed to map old to new list-valued parameter value.");
        }
      }

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
    }

    if (!topo.static_.IsOutput() &&
      !topo.static_.IsFakeParam() &&
      (!patchOnly || topo.static_.StoreInPatch()))
      *container.Params()[iter->second] = static_cast<float>(normalized.value());
  }

  FB_LOG_INFO("Start deserialization post-processing.");
  converter->PostProcess(isGuiState, oldParamInfos, container.Params());
  FB_LOG_INFO("End deserialization post-processing.");

  bool found = false;
  FB_LOG_INFO("Checking parameter indices...");
  for (int i = 0; i < oldParamInfos.size() && i < params.params.size(); i++)
    if (oldParamInfos[i].tag != params.params[i].tag)
    {
      found = true;
      FB_LOG_WARN("PARAMETER INDICES HAVE CHANGED.");
      FB_LOG_WARN("If your host does not support this, you need to re-setup automation lanes.");
      break;
    }
  if (!found)
    FB_LOG_INFO("Parameter indices are stable.");

  return true;
}