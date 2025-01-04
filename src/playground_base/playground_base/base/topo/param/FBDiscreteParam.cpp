#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

float
FBDiscreteParam::PlainToNormalized(int plain) const
{
  return std::clamp(plain / (valueCount - 1.0f), 0.0f, 1.0f);
}

std::string
FBDiscreteParam::PlainToText(int plain) const
{
  if (toText)
    return toText(plain);
  return std::to_string(plain);
}

std::optional<int>
FBDiscreteParam::TextToPlain(std::string const& text) const
{
  if (toText)
  {
    for (int i = 0; i < valueCount; i++)
      if (text == toText(i))
        return { i };
    return {};
  }

  char* end;
  unsigned long plain = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
    return {};
  int result = static_cast<int>(plain);
  if (result < 0 || result >= valueCount)
    return { };
  return { result };
}