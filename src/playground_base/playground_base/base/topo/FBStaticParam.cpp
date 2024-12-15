#include <playground_base/base/topo/FBStaticParam.hpp>

#include <cassert>

std::optional<int>
FBStaticParam::TextToDiscreteBool(std::string const& text) const
{
  if (text != "On" && text != "Off")
    return {};
  return { text == "On" ? 1 : 0 };
}

std::optional<int>
FBStaticParam::TextToDiscreteInt(std::string const& text) const
{
  char* end;
  unsigned long discrete = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
    return false;
  int result = static_cast<int>(discrete);
  if (result < 0 || result >= valueCount)
    return { };
  return { result };
}

std::optional<int>
FBStaticParam::TextToDiscreteList(std::string const& text, bool io) const
{
  for (int i = 0; i < list.size(); i++)
    if (text == (io ? list[i].id : list[i].text))
      return { i };
  return {};
}

std::optional<float>
FBStaticParam::TextToPlain(std::string const& text) const
{
  if (!percentage)
    return TextToPlainLinear(text, plainMin, plainMax);
  auto plain = TextToPlainLinear(text, 0.0f, 100.0f);
  if (!plain)
    return {};
  return { plain.value() / 100.0f };
}

std::optional<int>
FBStaticParam::TextToDiscrete(std::string const& text, bool io) const
{
  if (list.size() != 0)
    return TextToDiscreteList(text, io);
  else if (valueCount == 2)
    return TextToDiscreteBool(text);
  else
    return TextToDiscreteInt(text);
}

std::optional<float>
FBStaticParam::TextToPlainLinear(std::string const& text, float min, float max) const
{
  char* end;
  float result = std::strtof(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < min || result > max)
    return {};
  return { result };
}

std::string
FBStaticParam::PlainToText(float plain) const
{
  if (percentage)
    return std::to_string(plain * 100.0f);
  return std::to_string(plain);
}

std::string
FBStaticParam::DiscreteToText(int discrete, bool io) const
{
  if (list.size() != 0)
    return io ? list[discrete].id : list[discrete].text;
  if (valueCount == 2)
    return discrete == 0 ? "Off" : "On";
  return std::to_string(discrete);
}

std::string
FBStaticParam::NormalizedToText(bool io, float normalized) const
{
  assert(valueCount != 1);
  assert(list.size() == 0 || list.size() == valueCount);

  if (valueCount == 0)
    return PlainToText(NormalizedToPlainLinear(normalized));
  return DiscreteToText(NormalizedToDiscrete(normalized), io);
}

std::optional<float>
FBStaticParam::TextToNormalized(std::string const& text, bool io) const
{
  assert(valueCount != 1);
  assert(list.size() == 0 || list.size() == valueCount);

  if (valueCount == 0)
  {
    auto plain = TextToPlain(text);
    if (!plain)
      return {};
    return PlainLinearToNormalized(plain.value());
  }

  auto discrete = TextToDiscrete(text, io);
  if (!discrete)
    return {};
  return DiscreteToNormalized(discrete.value());
}