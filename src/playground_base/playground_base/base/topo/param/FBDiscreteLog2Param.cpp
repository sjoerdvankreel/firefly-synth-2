#include <playground_base/base/topo/param/FBDiscreteLog2Param.hpp>

#include <bit>

bool
FBDiscreteLog2ParamNonRealTime::IsItems() const
{
  return false;
}

int
FBDiscreteLog2ParamNonRealTime::ValueCount() const
{
  return valueCount;
}

double 
FBDiscreteLog2ParamNonRealTime::PlainToNormalized(double plain) const
{
  plain = std::bit_width((unsigned int)plain) - 1;
  return std::clamp(plain / (valueCount - 1.0), 0.0, 1.0);
}

double
FBDiscreteLog2ParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return FBDiscreteLog2ParamRealTime::NormalizedToPlain((float)normalized);
}

std::string
FBDiscreteLog2ParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const
{
  return std::to_string(plain);
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