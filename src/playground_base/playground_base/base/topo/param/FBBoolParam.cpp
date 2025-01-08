#include <playground_base/base/topo/param/FBBoolParam.hpp>

double
FBBoolParam::PlainToNormalized(bool plain) const
{
  return plain ? 1.0f : 0.0f;
}

std::string 
FBBoolParam::PlainToText(bool plain) const
{
  return plain ? "On" : "Off";
}

std::optional<bool> 
FBBoolParam::TextToPlain(std::string const& text) const
{
  if (text == "On")
    return true;
  if (text == "Off")
    return false;
  return {};
}