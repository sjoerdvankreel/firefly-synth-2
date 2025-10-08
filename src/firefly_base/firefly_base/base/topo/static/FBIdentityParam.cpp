#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBIdentityParam.hpp>

bool FBIdentityParamNonRealTime::IsItems() const { return false; }
bool FBIdentityParamNonRealTime::IsStepped() const { return false; }
int FBIdentityParamNonRealTime::ValueCount() const { return 0; }
int FBIdentityParamNonRealTime::ValueOffset() const { return 0; }
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
FBIdentityParamNonRealTime::PlainToText(bool io, int /*moduleIndex*/, double plain) const
{
  double displayPlain = plain;
  if (displayAsBipolar)
    displayPlain = FBToBipolar(displayPlain);
  displayPlain *= displayMultiplier;
  if (io)
    return std::to_string(displayPlain);
  return FBFormatDouble(displayPlain, FBDefaultDisplayPrecision);
}

std::optional<double>
FBIdentityParamNonRealTime::TextToPlainInternal(bool /*io*/, int /*moduleIndex*/, std::string const& text) const
{
  auto resultOpt = FBStringToDoubleOptCLocale(text);
  if (!resultOpt)
    return std::nullopt;
  double result = resultOpt.value();
  result /= displayMultiplier;
  if (displayAsBipolar)
    result = FBToUnipolar(result);
  if (result < 0.0 || result > 1.0)
    return {};
  return { result };
}