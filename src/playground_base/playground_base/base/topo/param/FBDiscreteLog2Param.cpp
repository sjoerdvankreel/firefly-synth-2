#include <playground_base/base/topo/param/FBDiscreteLog2Param.hpp>
#include <bit>

std::string
FBDiscreteLog2Param::PlainToText(int plain) const
{
  return std::to_string(plain);
}

float
FBDiscreteLog2Param::PlainToNormalized(int plain) const
{
  plain = std::bit_width((unsigned int)plain) - 1;
  return std::clamp(plain / (valueCount - 1.0f), 0.0f, 1.0f);
}

std::optional<int>
FBDiscreteLog2Param::TextToPlain(std::string const& text) const
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