#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBIdentityParam.hpp>

bool FBIdentityParamNonRealTime::IsItems() const { return false; }
bool FBIdentityParamNonRealTime::IsStepped() const { return false; }
int FBIdentityParamNonRealTime::ValueCount() const { return 0; }
FBEditType FBIdentityParamNonRealTime::GUIEditType() const { return FBEditType::Linear; }
FBEditType FBIdentityParamNonRealTime::AutomationEditType() const { return FBEditType::Linear; }

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
FBIdentityParamNonRealTime::PlainToText(bool io, double plain) const
{
  double displayPlain = plain * displayMultiplier;
  if (io)
    return std::to_string(displayPlain);
  return FBFormatDouble(displayPlain, FBDefaultDisplayPrecision);
}

std::optional<double>
FBIdentityParamNonRealTime::TextToPlain(bool io, std::string const& text) const
{
  char* end;
  double result = std::strtod(text.c_str(), &end);
  if (end != text.c_str() + text.size())
  {
    FB_LOG_WARN(std::string("Parsing text remainder: '") + end + "'.");
    return {};
  }
  result /= displayMultiplier;
  if (result < 0.0 || result > 1.0)
    return {};
  return { result };
}