#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
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
ParseColorScheme(
  DynamicObject const* obj, 
  FBColorScheme& result)
{
  result = {};

  if (!RequireStringProperty(obj, "background1"))
    return false;
  result.background1 = Colour::fromString(obj->getProperty("background1").toString());

  if (!RequireStringProperty(obj, "background2"))
    return false;
  result.background2 = Colour::fromString(obj->getProperty("background2").toString());

  if (!RequireStringProperty(obj, "foreground1"))
    return false;
  result.foreground1 = Colour::fromString(obj->getProperty("foreground1").toString());

  if (!RequireStringProperty(obj, "foreground2"))
    return false;
  result.foreground2 = Colour::fromString(obj->getProperty("foreground2").toString());

  if (!RequireStringProperty(obj, "highlight1"))
    return false;
  result.highlight1 = Colour::fromString(obj->getProperty("highlight1").toString());

  if (!RequireStringProperty(obj, "highlight2"))
    return false;
  result.highlight2 = Colour::fromString(obj->getProperty("highlight2").toString());

  return true;
}

static bool
ParseColorSchemesJson(
  DynamicObject const* obj, 
  std::map<std::string, FBColorScheme>& result)
{
  result = {};
  for (auto i = obj->getProperties().begin(); i != obj->getProperties().end(); i++)
  {
    if (!RequireObjectProperty(obj, i->name.toString()))
      return false;
    if (!ParseColorScheme(
      i->value.getDynamicObject(), 
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

  if (!RequireObjectProperty(obj, "colorSchemes"))
    return false;
  if (!ParseColorSchemesJson(obj->getProperty("colorSchemes").getDynamicObject(), result.colorSchemes))
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

std::vector<FBTheme>
FBLoadThemes(FBRuntimeTopo const* topo)
{
  (void)topo;
  std::vector<FBTheme> result = {};
  LoadThemeJsons();
  return result;
}