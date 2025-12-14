#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>

#include <juce_core/juce_core.h>

using namespace juce;

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
ParseColorScheme(
  DynamicObject const* obj, 
  FBColorScheme& result)
{
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
ParseColorSchemes(DynamicObject const* obj, std::map<std::string, FBColorScheme>& result)
{
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
ParseParamColorSchemes(
  DynamicObject const* obj,
  std::map<std::string, std::string>& result)
{
  for (auto i = obj->getProperties().begin(); i != obj->getProperties().end(); i++)
  {
    if (!RequireStringProperty(obj, i->name.toString()))
      return false;
    result[i->name.toString().toStdString()] = i->value.toString().toStdString();
  }
  return true;
}

static bool
ParseModuleColors(
  DynamicObject const* obj,
  FBModuleColors& result)
{
  if (!RequireStringProperty(obj, "colorScheme"))
    return false;
  result.colorScheme = obj->getProperty("colorScheme").toString().toStdString();

  if (!RequireObjectProperty(obj, "paramColorSchemes"))
    return false;
  if (!ParseParamColorSchemes(obj->getProperty("paramColorSchemes").getDynamicObject(), result.paramColorSchemes))
    return false;

  return true;
}

static bool
ParseModuleColors(
  DynamicObject const* obj,
  std::map<std::string, FBModuleColors>& result)
{
  for (auto i = obj->getProperties().begin(); i != obj->getProperties().end(); i++)
  {
    if (!RequireObjectProperty(obj, i->name.toString()))
      return false;
    if (!ParseModuleColors(
      obj->getProperty(i->name.toString()).getDynamicObject(), 
      result[i->name.toString().toStdString()]))
      return false;
  }
  return true;
}

static bool
ParseTheme(std::string const& jsonText, FBTheme& theme)
{
  theme = {};
  var json;
  if (!FBParseJson(jsonText, json))
    return false;
  DynamicObject const* obj = json.getDynamicObject();

  if (!RequireStringProperty(obj, "name"))
    return false;
  theme.name = obj->getProperty("name").toString().toStdString();

  if (!RequireObjectProperty(obj, "colorSchemes"))
    return false;
  if (!ParseColorSchemes(obj->getProperty("colorSchemes").getDynamicObject(), theme.colorSchemes))
    return false;

  if (!RequireObjectProperty(obj, "moduleColors"))
    return false;
  if (!ParseModuleColors(obj->getProperty("moduleColors").getDynamicObject(), theme.moduleColors))
    return false;

  // todo validate against topo
  return true;
}

std::map<std::string, FBTheme>
FBLoadThemes()
{

}