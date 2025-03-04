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
  auto plain = static_cast<double>(1 << std::clamp((int)std::round(normalized * valueCount), 0, valueCount - 1));
  auto norm = PlainToNormalized(plain);
  //auto plain2 = NormalizedToPlain(norm);
  return plain;
}

std::string
FBDiscreteLog2ParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const 
{
  return std::to_string((int)std::round(plain));
}

double
FBDiscreteLog2ParamNonRealTime::PlainToNormalized(double plain) const
{
  int linear = 0;
  unsigned discrete = static_cast<unsigned>(std::round(plain));
  assert(discrete > 0);
  assert(std::has_single_bit(discrete));
  while (discrete != 1)
  {
    linear++;
    discrete /= 2;
  }
  auto norm = std::clamp(linear / (valueCount - 1.0), 0.0, 1.0);
  //auto pln = NormalizedToPlain(norm);
  return norm;
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