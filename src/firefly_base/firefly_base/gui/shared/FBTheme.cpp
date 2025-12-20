#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <juce_core/juce_core.h>

using namespace juce;

struct FBParamColorsJson
{
  int paramSlot = -1; // -1 = all
  std::string paramId = {};
  std::string colorScheme = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBParamColorsJson);
};  

struct FBModuleColorsJson
{
  int moduleSlot = -1; // -1 = all
  std::string moduleId = {};
  std::string colorScheme = {};
  std::vector<FBParamColorsJson> guiParamColorSchemes = {};
  std::vector<FBParamColorsJson> audioParamColorSchemes = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBModuleColorsJson);
};

struct FBThemeJson
{
  std::string name = {};
  FBColorScheme defaultColorScheme = {};
  std::vector<FBModuleColorsJson> moduleColors = {};
  std::map<std::string, FBColorScheme> colorSchemes = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBThemeJson);
};

static bool
RequireProperty(
  DynamicObject const* obj, 
  String const& name)
{
  if (!obj->hasProperty(name))
  {
    FB_LOG_ERROR("Missing json property '" + name.toStdString() + "'.");
    return false;
  }
  return true;
}

static bool
RequireIntProperty(
  DynamicObject const* obj,
  String const& name)
{
  if (!RequireProperty(obj, name))
    return false;
  if (!obj->getProperty(name).isInt())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not an integer.");
    return false;
  }
  return true;
}

static bool
RequireDoubleProperty(
  DynamicObject const* obj,
  String const& name)
{
  if (!RequireProperty(obj, name))
    return false;
  if (!obj->getProperty(name).isDouble())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not a double.");
    return false;
  }
  return true;
}

static bool
RequireStringProperty(
  DynamicObject const* obj, 
  String const& name)
{
  if (!RequireProperty(obj, name))
    return false;
  if (!obj->getProperty(name).isString())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not a string.");
    return false;
  }
  return true;
}

static bool
RequireObjectProperty(
  DynamicObject const* obj, 
  String const& name)
{
  if (!RequireProperty(obj, name))
    return false;
  if (!obj->getProperty(name).isObject())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not an object.");
    return false;
  }
  return true;
}

static bool
RequireArrayProperty(
  DynamicObject const* obj,
  String const& name)
{
  if (!RequireProperty(obj, name))
    return false;
  if (!obj->getProperty(name).isArray())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not an array.");
    return false;
  }
  return true;
}

static bool
OptionalStringProperty(
  DynamicObject const* obj,
  String const& name,
  bool& present)
{
  present = false;
  if (!obj->hasProperty(name))
    return true;
  if (!obj->getProperty(name).isString())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not a string.");
    return false;
  }
  present = true;
  return true;
}

static bool
OptionalArrayProperty(
  DynamicObject const* obj,
  String const& name,
  bool& present)
{
  present = false;
  if (!obj->hasProperty(name))
    return true;
  if (!obj->getProperty(name).isArray())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not an array.");
    return false;
  }
  present = true;
  return true;
}

static bool
OptionalDoubleProperty(
  DynamicObject const* obj,
  String const& name,
  bool& present)
{
  present = false;
  if (!obj->hasProperty(name))
    return true;
  if (!obj->getProperty(name).isDouble())
  {
    FB_LOG_ERROR("Json property '" + name.toStdString() + "' is not a double.");
    return false;
  }
  present = true;
  return true;
}

static bool
ParseDefaultColorScheme(
  DynamicObject const* obj, 
  FBColorScheme& result)
{
  result = {};

  if (!RequireDoubleProperty(obj, "dimDisabled"))
    return false;
  result.dimDisabled = (float)(double)obj->getProperty("dimDisabled");
  if (result.dimDisabled < 0.0 || result.dimDisabled > 1.0)
  {
    FB_LOG_ERROR("Dim disabled should be between 0 and 1.");
    return false;
  }

  if (!RequireStringProperty(obj, "background"))
    return false;
  result.background = Colour::fromString(obj->getProperty("background").toString());

  if (!RequireStringProperty(obj, "graphGrid"))
    return false;
  result.graphGrid = Colour::fromString(obj->getProperty("graphGrid").toString());
  
  if (!RequireStringProperty(obj, "graphBackground"))
    return false;
  result.graphBackground = Colour::fromString(obj->getProperty("graphBackground").toString());

  if (!RequireStringProperty(obj, "sectionBorder"))
    return false;
  result.sectionBorder = Colour::fromString(obj->getProperty("sectionBorder").toString());

  if (!RequireStringProperty(obj, "sectionBackground"))
    return false;
  result.sectionBackground = Colour::fromString(obj->getProperty("sectionBackground").toString());

  if (!RequireStringProperty(obj, "sectionText"))
    return false;
  result.sectionText = Colour::fromString(obj->getProperty("sectionText").toString());

  if (!RequireStringProperty(obj, "paramSecondary"))
    return false;
  result.paramSecondary = Colour::fromString(obj->getProperty("paramSecondary").toString());

  if (!RequireStringProperty(obj, "paramPrimary"))
    return false;
  result.paramPrimary = Colour::fromString(obj->getProperty("paramPrimary").toString());

  if (!RequireStringProperty(obj, "paramHighlight"))
    return false;
  result.paramHighlight = Colour::fromString(obj->getProperty("paramHighlight").toString());

  if (!RequireStringProperty(obj, "paramBackground"))
    return false;
  result.paramBackground = Colour::fromString(obj->getProperty("paramBackground").toString());

  return true;
}

static bool
ParseColorScheme(
  DynamicObject const* obj,
  FBColorScheme const& defaultScheme,
  FBColorScheme& result)
{
  result = {};
  bool present = false;

  if (!OptionalDoubleProperty(obj, "dimDisabled", present))
    return false;
  result.dimDisabled = present ? (float)(double)obj->getProperty("dimDisabled") : defaultScheme.dimDisabled;
  if (result.dimDisabled < 0.0 || result.dimDisabled > 1.0)
  {
    FB_LOG_ERROR("Dim disabled should be between 0 and 1.");
    return false;
  }

  if (!OptionalStringProperty(obj, "background", present))
    return false;
  result.background = present ? Colour::fromString(obj->getProperty("background").toString()) : defaultScheme.background;

  if (!OptionalStringProperty(obj, "graphGrid", present))
    return false;
  result.graphGrid = present ? Colour::fromString(obj->getProperty("graphGrid").toString()) : defaultScheme.graphGrid;

  if (!OptionalStringProperty(obj, "graphBackground", present))
    return false;
  result.graphBackground = present ? Colour::fromString(obj->getProperty("graphBackground").toString()) : defaultScheme.graphBackground;

  if (!OptionalStringProperty(obj, "sectionBorder", present))
    return false;
  result.sectionBorder = present ? Colour::fromString(obj->getProperty("sectionBorder").toString()) : defaultScheme.sectionBorder;

  if (!OptionalStringProperty(obj, "sectionBackground", present))
    return false;
  result.sectionBackground = present ? Colour::fromString(obj->getProperty("sectionBackground").toString()) : defaultScheme.sectionBackground;

  if (!OptionalStringProperty(obj, "sectionText", present))
    return false;
  result.sectionText = present ? Colour::fromString(obj->getProperty("sectionText").toString()) : defaultScheme.sectionText;

  if (!OptionalStringProperty(obj, "paramSecondary", present))
    return false;
  result.paramSecondary = present ? Colour::fromString(obj->getProperty("paramSecondary").toString()) : defaultScheme.paramSecondary;

  if (!OptionalStringProperty(obj, "paramPrimary", present))
    return false;
  result.paramPrimary = present ? Colour::fromString(obj->getProperty("paramPrimary").toString()) : defaultScheme.paramPrimary;

  if (!OptionalStringProperty(obj, "paramHighlight", present))
    return false;
  result.paramHighlight = present ? Colour::fromString(obj->getProperty("paramHighlight").toString()) : defaultScheme.paramHighlight;

  if (!OptionalStringProperty(obj, "paramBackground", present))
    return false;
  result.paramBackground = present ? Colour::fromString(obj->getProperty("paramBackground").toString()) : defaultScheme.paramBackground;

  return true;
}

static bool
ParseColorSchemesJson(
  DynamicObject const* obj, 
  FBColorScheme const& defaultScheme,
  std::map<std::string, FBColorScheme>& result)
{
  result = {};
  for (auto i = obj->getProperties().begin(); i != obj->getProperties().end(); i++)
  {
    if (!RequireObjectProperty(obj, i->name.toString()))
      return false;
    if (!ParseColorScheme(
      i->value.getDynamicObject(), 
      defaultScheme,
      result[i->name.toString().toStdString()]))
      return false;
  }
  return true;
}

static bool
ParseParamColorSchemeJson(
  DynamicObject const* obj,
  FBParamColorsJson& result)
{
  result = {};
  if (!RequireIntProperty(obj, "paramSlot"))
    return false;
  result.paramSlot = (int)obj->getProperty("paramSlot");
  if (result.paramSlot < -1)
  {
    FB_LOG_ERROR("Invalid value '" + std::to_string(result.paramSlot) + "'");
    return false;
  }
  if (!RequireStringProperty(obj, "paramId"))
    return false;
  result.paramId = obj->getProperty("paramId").toString().toStdString();
  if (!RequireStringProperty(obj, "colorScheme"))
    return false;
  result.colorScheme = obj->getProperty("colorScheme").toString().toStdString();
  return true;
} 

static bool
ParseParamColorSchemesJson(
  juce::var const& json,
  std::vector<FBParamColorsJson>& result)
{
  result.clear();
  for (int i = 0; i < json.size(); i++)
  {
    if (!json[i].isObject())
      return false;
    FBParamColorsJson paramColors = {};
    if (!ParseParamColorSchemeJson(json[i].getDynamicObject(), paramColors))
      return false;
    result.push_back(paramColors);
  }
  return true;
}

static bool
ParseModuleColorsJson(
  DynamicObject const* obj,
  FBModuleColorsJson& result)
{
  bool present = false;

  if (!RequireStringProperty(obj, "colorScheme"))
    return false;
  result.colorScheme = obj->getProperty("colorScheme").toString().toStdString();

  if (!RequireStringProperty(obj, "moduleId"))
    return false;
  result.moduleId = obj->getProperty("moduleId").toString().toStdString();
  
  if (!RequireIntProperty(obj, "moduleSlot"))
    return false;
  result.moduleSlot = (int)obj->getProperty("moduleSlot");
  if (result.moduleSlot < -1)
  {
    FB_LOG_ERROR("Invalid value '" + std::to_string(result.moduleSlot) + "'");
    return false;
  }

  if (!OptionalArrayProperty(obj, "guiParamColorSchemes", present))
    return false;
  if (present && !ParseParamColorSchemesJson(obj->getProperty("guiParamColorSchemes"), result.guiParamColorSchemes))
    return false;

  if (!OptionalArrayProperty(obj, "audioParamColorSchemes", present))
    return false;
  if (present && !ParseParamColorSchemesJson(obj->getProperty("audioParamColorSchemes"), result.audioParamColorSchemes))
    return false;

  return true;
}

static bool
ParseModuleColorsJson(
  var const& json,
  std::vector<FBModuleColorsJson>& result)
{
  result.clear();
  for (int i = 0; i < json.size(); i++)
  {
    if(!json[i].isObject())
      return false;
    FBModuleColorsJson moduleColors = {};
    if (!ParseModuleColorsJson(json[i].getDynamicObject(), moduleColors))
      return false;
    result.push_back(moduleColors);
  }
  return true;
}

static bool
ParseThemeJson(String const& jsonText, FBThemeJson& result)
{
  var json;
  result = {};
  
  if (!FBParseJson(jsonText.toStdString(), json))
    return false;
  DynamicObject const* obj = json.getDynamicObject();

  if (!RequireStringProperty(obj, "name"))
    return false;
  result.name = obj->getProperty("name").toString().toStdString();

  if (!RequireObjectProperty(obj, "defaultColorScheme"))
    return false;
  if (!ParseDefaultColorScheme(obj->getProperty("defaultColorScheme").getDynamicObject(), result.defaultColorScheme))
    return false;

  if (!RequireObjectProperty(obj, "colorSchemes"))
    return false;
  if (!ParseColorSchemesJson(obj->getProperty("colorSchemes").getDynamicObject(), result.defaultColorScheme, result.colorSchemes))
    return false;

  if (!RequireArrayProperty(obj, "moduleColors"))
    return false;
  if (!ParseModuleColorsJson(obj->getProperty("moduleColors"), result.moduleColors))
    return false;

  for(int i = 0; i < result.moduleColors.size(); i++)
  {
    auto const& moduleScheme = result.moduleColors[i].colorScheme;
    if (result.colorSchemes.find(moduleScheme) == result.colorSchemes.end())
    {
      FB_LOG_ERROR("Color scheme '" + moduleScheme + "' not found.");
      return false;
    }
    for(int j = 0; j < result.moduleColors[i].guiParamColorSchemes.size(); j++)
    {
      auto const& paramScheme = result.moduleColors[i].guiParamColorSchemes[j].colorScheme;
      if (result.colorSchemes.find(paramScheme) == result.colorSchemes.end())
      {
        FB_LOG_ERROR("Color scheme '" + paramScheme + "' not found.");
        return false;
      }
    }
    for (int j = 0; j < result.moduleColors[i].audioParamColorSchemes.size(); j++)
    {
      auto const& paramScheme = result.moduleColors[i].audioParamColorSchemes[j].colorScheme;
      if (result.colorSchemes.find(paramScheme) == result.colorSchemes.end())
      {
        FB_LOG_ERROR("Color scheme '" + paramScheme + "' not found.");
        return false;
      }
    }
  }
  
  return true;
}

static std::vector<FBThemeJson>
LoadThemeJsons()
{
  std::filesystem::path themeRoot(FBGetResourcesFolderPath() / "ui" / "themes");
  if (!std::filesystem::exists(themeRoot))
    return {};

  std::vector<FBThemeJson> result = {};
  for (auto const& i : std::filesystem::directory_iterator(themeRoot))
    if (std::filesystem::is_regular_file(i.path()))
      if (i.path().has_extension() && i.path().extension().string() == ".json")
      {
        File file(String(i.path().string()));
        if (file.exists())
        {
          FB_LOG_INFO("Loading theme file '" + i.path().string() + "'.");
          FBThemeJson themeJson = {};
          if (ParseThemeJson(file.loadFileAsString(), themeJson))
          {
            bool foundName = false;
            for (int j = 0; j < result.size(); j++)
              if (result[j].name == themeJson.name)
              {
                foundName = true;
                break;
              }
            if (!foundName)
              result.push_back(themeJson);
          }
          FB_LOG_INFO("Load theme file '" + i.path().string() + "'.");
        }
      }
  return result;
}

template <class TParam>
static bool
MakeParamColors(
  std::vector<FBParamColorsJson> const& paramColorSchemes,
  std::vector<TParam> const& params,
  std::vector<std::vector<int>> const& moduleParamTopoToRuntime,
  std::map<int, std::string>& result)
{
  result = {};

  for (int i = 0; i < paramColorSchemes.size(); i++)
  {
    int foundIndex = -1;
    for (int j = 0; j < params.size(); j++)
      if (FBCleanTopoId(params[j].id) == paramColorSchemes[i].paramId)
      {
        foundIndex = j;
        break;
      }
    if (foundIndex == -1)
    {
      FB_LOG_ERROR("Cannot find param '" + paramColorSchemes[i].paramId + "'.");
      return false;
    }
    if (paramColorSchemes[i].paramSlot >= params[foundIndex].slotCount)
    {
      FB_LOG_ERROR("Invalid param slot '" + std::to_string(paramColorSchemes[i].paramSlot) + "'.");
      return false;
    }

    for (int j = 0; j < params[foundIndex].slotCount; j++)
      if (paramColorSchemes[i].paramSlot == -1 || paramColorSchemes[i].paramSlot == j)
      {
        int rtIndex = moduleParamTopoToRuntime[foundIndex][j];
        result[rtIndex] = paramColorSchemes[i].colorScheme;
      }
  }

  return true;
}

static bool
MakeModuleColors(
  FBRuntimeTopo const* topo,
  int staticModuleIndex,
  int staticModuleSlot,
  FBModuleColorsJson const& json,
  FBModuleColors& result)
{
  result = {};
  result.colorScheme = json.colorScheme;
  if (!MakeParamColors(
    json.audioParamColorSchemes,
    topo->static_->modules[staticModuleIndex].params,
    topo->audio.paramTopoToRuntime[staticModuleIndex][staticModuleSlot],
    result.audioParamColorSchemes))
    return false;
  if (!MakeParamColors(
    json.guiParamColorSchemes,
    topo->static_->modules[staticModuleIndex].guiParams,
    topo->gui.paramTopoToRuntime[staticModuleIndex][staticModuleSlot],
    result.guiParamColorSchemes))
    return false;
  return true;
}

static bool
MakeTheme(
  FBRuntimeTopo const* topo,
  FBThemeJson const& themeJson,
  FBTheme& theme)
{
  theme = {};
  theme.name = themeJson.name;
  theme.colorSchemes = themeJson.colorSchemes;
  theme.defaultColorScheme = FBColorScheme(themeJson.defaultColorScheme);
  for (int i = 0; i < themeJson.moduleColors.size(); i++)
  {
    int foundIndex = -1;
    for (int j = 0; j < topo->static_->modules.size(); j++)
      if (FBCleanTopoId(topo->static_->modules[j].id) == themeJson.moduleColors[i].moduleId)
      {
        foundIndex = j;
        break;
      }
    if (foundIndex == -1)
    {
      FB_LOG_ERROR("Cannot find module '" + themeJson.moduleColors[i].moduleId + "'.");
      return false;
    }
    if (themeJson.moduleColors[i].moduleSlot >= topo->static_->modules[foundIndex].slotCount)
    {
      FB_LOG_ERROR("Invalid module slot '" + std::to_string(themeJson.moduleColors[i].moduleSlot) + "'.");
      return false;
    } 

    for (int j = 0; j < topo->static_->modules[foundIndex].slotCount; j++)
      if (themeJson.moduleColors[i].moduleSlot == -1 || themeJson.moduleColors[i].moduleSlot == j)
      {
        FBModuleColors moduleColors = {};
        if (!MakeModuleColors(topo, foundIndex, j, themeJson.moduleColors[i], moduleColors))
          return false;
        int rtIndex = topo->moduleTopoToRuntime.at({ foundIndex, j });
        theme.moduleColors[rtIndex] = FBModuleColors(moduleColors);
      }
  }
  return true;
}

std::vector<FBTheme>
FBLoadThemes(FBRuntimeTopo const* topo)
{
  (void)topo;
  std::vector<FBTheme> result = {};
  auto themeJsons = LoadThemeJsons();
  for (int i = 0; i < themeJsons.size(); i++)
  {
    FBTheme theme = {};
    if (MakeTheme(topo, themeJsons[i], theme))
      result.push_back(theme);
  }
  return result;
}