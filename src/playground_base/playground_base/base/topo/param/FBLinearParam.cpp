#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>

double 
FBLinearParamNonRealTime::PlainToNormalized(double plain) const
{
  return std::clamp((plain - min) / (max - min), 0.0, 1.0);
}

double
FBLinearParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return FBLinearParamRealTime::NormalizedToPlain((float)normalized);
}

std::string
FBLinearParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const
{
  double displayPlain = plain * displayMultiplier;
  if (display == FBValueTextDisplay::IO)
    return std::to_string(displayPlain);
  return FBFormatFloat(displayPlain, FBDefaultDisplayPrecision);
}

std::optional<double>
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