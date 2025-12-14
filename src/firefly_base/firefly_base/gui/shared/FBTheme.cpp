#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <juce_core/juce_core.h>

using namespace juce;

bool
FBThemeLoad(std::string const& jsonText, FBTheme& theme)
{
  theme = {};
  var json;
  if (!FBParseJson(jsonText, json))
    return false;
  return false;
}