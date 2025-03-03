#include <playground_base/base/topo/param/FBBoolParam.hpp>

int
FBBoolParamNonRealTime::ValueCount() const
{
  return 2;
}

float 
FBBoolParamNonRealTime::PlainToNormalized(int plain) const
{
  return plain != 0 ? 1.0f : 0.0f;
}

int
FBBoolParamNonRealTime::NormalizedToPlain(float normalized) const
{
  return FBBoolParamRealTime::NormalizedToPlain(normalized)? 1: 0;
}

std::string
FBBoolParamNonRealTime::PlainToText(FBValueTextDisplay display, int plain) const
{
  return plain != 0 ? "On" : "Off";
}

std::optional<int>
FBBoolParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  if (text == "On")
    return 1;
  if (text == "Off")
    return 0;
  return {};
}