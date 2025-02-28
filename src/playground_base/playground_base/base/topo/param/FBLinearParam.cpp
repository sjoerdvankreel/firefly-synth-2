#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>

float 
FBLinearParamNonRealTime::PlainToNormalized(float plain) const
{
  return std::clamp((plain - min) / (max - min), 0.0f, 1.0f);
}

float
FBLinearParamNonRealTime::NormalizedToPlain(float normalized) const
{
  return FBLinearParamRealTime::NormalizedToPlain(normalized);
}

std::string
FBLinearParamNonRealTime::PlainToText(FBValueTextDisplay display, float plain) const
{
  float displayPlain = plain * displayMultiplier;
  if (display == FBValueTextDisplay::IO)
    return std::to_string(displayPlain);
  return FBFormatFloat(displayPlain, FBDefaultDisplayPrecision);
}

std::optional<float>
FBLinearParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  char* end;
  float result = std::strtof(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  result /= displayMultiplier;
  if (result < min || result > max)
    return {};
  return { result };
}