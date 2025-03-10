#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/param/FBIdentityParam.hpp>

bool 
FBIdentityParamNonRealTime::IsItems() const
{
  return false;
}

bool 
FBIdentityParamNonRealTime::IsStepped() const
{
  return false;
}

int 
FBIdentityParamNonRealTime::ValueCount() const
{
  return 0;
}

FBEditType
FBIdentityParamNonRealTime::GUIEditType() const
{
  return FBEditType::Linear;
}

FBEditType
FBIdentityParamNonRealTime::AutomationEditType() const
{
  return FBEditType::Linear;
}

double 
FBIdentityParamNonRealTime::PlainToNormalized(double plain) const
{
  return plain;
}

double
FBIdentityParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return normalized;
}

std::string
FBIdentityParamNonRealTime::PlainToText(FBTextDisplay display, double plain) const
{
  double displayPlain = plain * displayMultiplier;
  if (display == FBTextDisplay::IO)
    return std::to_string(displayPlain);
  return FBFormatDouble(displayPlain, FBDefaultDisplayPrecision);
}

std::optional<double>
FBIdentityParamNonRealTime::TextToPlain(FBTextDisplay display, std::string const& text) const
{
  char* end;
  double result = std::strtod(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  result /= displayMultiplier;
  if (result < 0.0 || result > 1.0)
    return {};
  return { result };
}