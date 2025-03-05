#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

bool 
FBDiscreteParamNonRealTime::IsItems() const
{
  return false;
}

bool 
FBDiscreteParamNonRealTime::IsStepped() const 
{
  return true;
}

int
FBDiscreteParamNonRealTime::ValueCount() const 
{
  return valueCount;
}

double 
FBDiscreteParamNonRealTime::PlainToNormalized(double plain) const 
{
  return std::clamp(plain / (valueCount - 1.0), 0.0, 1.0);
}

double
FBDiscreteParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  int scaled = static_cast<int>(normalized * valueCount);
  return std::clamp(scaled, 0, valueCount - 1);
}

std::string
FBDiscreteParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const 
{
  return std::to_string(static_cast<int>(std::round(plain)));
}

std::optional<double>
FBDiscreteParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const 
{
  char* end;
  unsigned long plain = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
    return {};
  if (static_cast<int>(plain) >= valueCount)
    return { };
  return { plain };
}