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
};  

struct FBModuleColorsJson
{
  int moduleSlot = -1; // -1 = all
  std::string moduleId = {};
  std::string colorScheme = {};
  std::vector<FBParamColorsJson> paramColorSchemes = {};
};

struct FBThemeJson
{
  std::string name = {};
  FBColorScheme defaultColorScheme = {};
  std::vector<FBModuleColorsJson> moduleColors = {};
  std::map<std::string, FBColorScheme> colorSchemes = {};
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
ParseDefaultColorScheme(
  DynamicObject const* obj, 
  FBColorScheme& result)
{
  result = {};

  if (!RequireStringProperty(obj, "border"))
    return false;
  result.border = Colour::fromString(obj->getProperty("border").toString());

  if (!RequireStringProperty(obj, "background"))
    return false;
  result.background = Colour::fromString(obj->getProperty("background").toString());

  if (!RequireStringProperty(obj, "foreground"))
    return false;
  result.foreground = Colour::fromString(obj->getProperty("foreground").toString());

  if (!RequireStringProperty(obj, "controlBounds"))
    return false;
  result.controlBounds = Colour::fromString(obj->getProperty("controlBounds").toString());

  if (!RequireStringProperty(obj, "controlEngine"))
    return false;
  result.controlEngine = Colour::fromString(obj->getProperty("controlEngine").toString());

  if (!RequireStringProperty(obj, "controlTweaked"))
    return false;
  result.controlTweaked = Colour::fromString(obj->getProperty("controlTweaked").toString());
   
  if (!RequireStringProperty(obj, "controlBorder"))
    return false;
  result.controlBorder = Colour::fromString(obj->getProperty("controlBorder").toString());

  if (!RequireStringProperty(obj, "controlBackground"))
    return false;
  result.controlBackground = Colour::fromString(obj->getProperty("controlBackground").toString());

  if (!RequireStringProperty(obj, "controlForeground"))
    return false;
  result.controlForeground = Colour::fromString(obj->getProperty("controlForeground").toString());

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

  if (!OptionalStringProperty(obj, "border", present))
    return false;
  result.border = present ? Colour::fromString(obj->getProperty("border").toString()) : defaultScheme.border;

  if (!OptionalStringProperty(obj, "background", present))
    return false;
  result.background = present ? Colour::fromString(obj->getProperty("background").toString()) : defaultScheme.background;

  if (!OptionalStringProperty(obj, "foreground", present))
    return false;
  result.foreground = present ? Colour::fromString(obj->getProperty("foreground").toString()) : defaultScheme.foreground;

  if (!OptionalStringProperty(obj, "controlBounds", present))
    return false;
  result.controlBounds = present ? Colour::fromString(obj->getProperty("controlBounds").toString()) : defaultScheme.controlBounds;

  if (!OptionalStringProperty(obj, "controlEngine", present))
    return false;
  result.controlEngine = present ? Colour::fromString(obj->getProperty("controlEngine").toString()) : defaultScheme.controlEngine;

  if (!OptionalStringProperty(obj, "controlTweaked", present))
    return false;
  result.controlTweaked = present ? Colour::fromString(obj->getProperty("controlTweaked").toString()) : defaultScheme.controlTweaked;

  if (!OptionalStringProperty(obj, "controlBorder", present))
    return false;
  result.controlBorder = present ? Colour::fromString(obj->getProperty("controlBorder").toString()) : defaultScheme.controlBorder;

  if (!OptionalStringProperty(obj, "controlBackground", present))
    return false;
  result.controlBackground = present ? Colour::fromString(obj->getProperty("controlBackground").toString()) : defaultScheme.controlBackground;

  if (!OptionalStringProperty(obj, "controlForeground", present))
    return false;
  result.controlForeground = present ? Colour::fromString(obj->getProperty("controlForeground").toString()) : defaultScheme.controlForeground;

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
  result = {};
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

  if (!RequireArrayProperty(obj, "paramColorSchemes"))
    return false;
  if (!ParseParamColorSchemesJson(obj->getProperty("paramColorSchemes"), result.paramColorSchemes))
    return false;

  return true;
}

static bool
ParseModuleColorsJson(
  var const& json,
  std::vector<FBModuleColorsJson>& result)
{
  result = {};
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
    for(int j = 0; j < result.moduleColors[i].paramColorSchemes.size(); j++)
    {
      auto const& paramScheme = result.moduleColors[i].paramColorSchemes[i].colorScheme;
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
  for (int i = 0; i < json.paramColorSchemes.size(); i++)
  {
    int foundIndex = -1;
    for (int j = 0; j < topo->static_->modules[staticModuleIndex].params.size(); j++)
      if (FBCleanTopoId(topo->static_->modules[staticModuleIndex].params[j].id) == json.paramColorSchemes[i].paramId)
      {       
        foundIndex = j;
        break;
      }
    if (foundIndex == -1)
    {
      FB_LOG_ERROR("Cannot find param '" + json.paramColorSchemes[i].paramId + "'.");
      return false;
    }
    if (json.paramColorSchemes[i].paramSlot >= topo->static_->modules[staticModuleIndex].params[foundIndex].slotCount)
    {
      FB_LOG_ERROR("Invalid param slot '" + std::to_string(json.paramColorSchemes[i].paramSlot) + "'.");
      return false;
    }

    // todo stuff with gui params
    for (int j = 0; j < topo->static_->modules[staticModuleIndex].params[foundIndex].slotCount; j++)
      if (json.paramColorSchemes[i].paramSlot == -1 || json.paramColorSchemes[i].paramSlot == j)
      {
        int rtIndex = topo->audio.paramTopoToRuntime[staticModuleIndex][staticModuleSlot][foundIndex][j];
        result.paramColorSchemes[rtIndex] = json.paramColorSchemes[i].colorScheme;
      }
  }
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
  theme.defaultColorScheme = themeJson.defaultColorScheme;
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
        theme.moduleColors[rtIndex] = moduleColors;
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