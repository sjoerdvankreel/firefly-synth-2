#include <firefly_base/base/topo/static/FBLinearParam.hpp>

bool FBLinearParamNonRealTime::IsItems() const { return false; }
bool FBLinearParamNonRealTime::IsStepped() const { return false; }
int FBLinearParamNonRealTime::ValueCount() const { return 0; }
FBEditType FBLinearParamNonRealTime::AutomationEditType() const { return FBEditType::Linear; }

FBEditType FBLinearParamNonRealTime::GUIEditType() const 
{ 
  return editSkewFactor == 1.0 ? FBEditType::Linear : FBEditType::Logarithmic; 
}

double 
FBLinearParamNonRealTime::PlainToNormalized(double plain) const 
{
  return std::clamp((plain - min) / (max - min), 0.0, 1.0);
}

double
FBLinearParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  return min + (max - min) * normalized;
}

std::string
FBLinearParamNonRealTime::PlainToText(bool io, double plain) const
{
  double displayPlain = plain * displayMultiplier;
  if (io)
    return std::to_string(displayPlain);
  return FBFormatDouble(displayPlain, FBDefaultDisplayPrecision);
}

std::optional<double>
FBLinearParamNonRealTime::TextToPlain(bool io, std::string const& text) const
{
  char* end;
  double result = std::strtod(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  result /= displayMultiplier;
  if (result < min || result > max)
    return {};
  return { result };
}