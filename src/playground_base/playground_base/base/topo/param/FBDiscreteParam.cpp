#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

std::string
FBDiscreteParam::PlainToText(int plain) const
{
  return std::to_string(plain);
}

float
FBDiscreteParam::PlainToNormalized(int plain) const
{
  return std::clamp(plain / (valueCount - 1.0f), 0.0f, 1.0f);
}

std::optional<int>
FBDiscreteParam::TextToPlain(std::string const& text) const
{
  char* end;
  unsigned long plain = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
    return {};
  int result = static_cast<int>(plain);
  if (result < 0 || result >= valueCount)
    return { };
  return { result };
}