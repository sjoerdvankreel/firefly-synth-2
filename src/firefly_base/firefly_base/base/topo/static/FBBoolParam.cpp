#include <firefly_base/base/topo/static/FBBoolParam.hpp>

int FBBoolParamNonRealTime::ValueCount() const { return 2; }
bool FBBoolParamNonRealTime::IsItems() const { return false; }
bool FBBoolParamNonRealTime::IsStepped() const { return true; }
FBEditType FBBoolParamNonRealTime::GUIEditType() const { return FBEditType::Stepped; }
FBEditType FBBoolParamNonRealTime::AutomationEditType() const { return FBEditType::Stepped; }

double 
FBBoolParamNonRealTime::PlainToNormalized(double plain) const
{
  return plain >= 0.5 ? 1.0 : 0.0;
}

double 
FBBoolParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return normalized >= 0.5 ? 1.0 : 0.0;
}

std::string 
FBBoolParamNonRealTime::PlainToText(bool io, double plain) const
{
  return plain >= 0.5 ? "On" : "Off";
}

std::optional<double>
FBBoolParamNonRealTime::TextToPlain(bool io, std::string const& text) const
{
  if (text == "On")
    return 1.0;
  if (text == "Off")
    return 0.0;
  return {};
}