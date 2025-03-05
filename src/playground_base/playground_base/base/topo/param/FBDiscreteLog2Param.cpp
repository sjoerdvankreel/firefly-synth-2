#include <playground_base/base/topo/param/FBDiscreteLog2Param.hpp>

#include <bit>
#include <cassert>

bool 
FBDiscreteLog2ParamNonRealTime::IsItems() const
{
  return false;
}

bool 
FBDiscreteLog2ParamNonRealTime::IsStepped() const 
{
  return true;
}

int
FBDiscreteLog2ParamNonRealTime::ValueCount() const 
{
  return valueCount;
}

double
FBDiscreteLog2ParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  int scaled = static_cast<int>(normalized * valueCount);
  return static_cast<double>(1 << std::clamp(scaled, 0, valueCount - 1));
}

std::string
FBDiscreteLog2ParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const 
{
  return std::to_string((int)std::round(plain));
}

double
FBDiscreteLog2ParamNonRealTime::PlainToNormalized(double plain) const
{
  unsigned width = std::bit_width(static_cast<unsigned>(plain) - 1);
  return std::clamp(width / (valueCount - 1.0), 0.0, 1.0);
}

std::optional<double>
FBDiscreteLog2ParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const 
{
  char* end;
  unsigned long plain = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
    return {};
  int result = static_cast<int>(plain);
  if (result < 1 || result > (1 << (valueCount - 1)))
    return { };
  return { result };
}