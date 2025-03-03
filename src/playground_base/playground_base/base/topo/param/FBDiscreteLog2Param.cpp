#include <playground_base/base/topo/param/FBDiscreteLog2Param.hpp>

#include <bit>

int 
FBDiscreteLog2ParamNonRealTime::ValueCount() const
{
  return valueCount;
}

float 
FBDiscreteLog2ParamNonRealTime::PlainToNormalized(int plain) const
{
  plain = std::bit_width((unsigned int)plain) - 1;
  return std::clamp(plain / (valueCount - 1.0f), 0.0f, 1.0f);
}

int
FBDiscreteLog2ParamNonRealTime::NormalizedToPlain(float normalized) const
{
  return FBDiscreteLog2ParamRealTime::NormalizedToPlain(normalized);
}

std::string
FBDiscreteLog2ParamNonRealTime::PlainToText(FBValueTextDisplay display, int plain) const
{
  return std::to_string(plain);
}

std::optional<int>
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