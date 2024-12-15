#include <playground_base/base/topo/FBStaticParam.hpp>

#include <cassert>

static std::optional<int>
TextToDiscreteBool(std::string const& text)
{
  if (text != "On" && text != "Off")
    return {};
  return { text == "On" ? 1 : 0 };
}

static std::optional<int>
TextToDiscreteInt(std::string const& text, int valueCount)
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

static std::optional<int>
TextToDiscreteList(std::string const& text, bool io, std::vector<FBListItem> const& list)
{
  for (int i = 0; i < list.size(); i++)
    if (text == (io ? list[i].id : list[i].text))
      return { i };
  return {};
}

static std::optional<float>
TextToPlainLinear(std::string const& text, float min, float max)
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
FBStaticParam::NormalizedToText(bool io, float normalized) const
{
  assert(valueCount != 1);
  assert(list.size() == 0 || list.size() == valueCount);

  if (valueCount == 0)
    if (percentage)
      return std::to_string(normalized * 100.0f);
    else
      return std::to_string(NormalizedToPlainLinear(normalized));

  int discrete = NormalizedToDiscrete(normalized);
  if (list.size() != 0)
    return io ? list[discrete].id : list[discrete].text;
  if (valueCount == 2)
    return discrete == 0 ? "Off" : "On";
  return std::to_string(discrete);
}

std::optional<float>
FBStaticParam::TextToNormalized(std::string const& text, bool io) const
{
  assert(valueCount != 1);
  assert(list.size() == 0 || list.size() == valueCount);

  if (valueCount == 0)
    if (percentage)
    {
      auto linearPercentage = TextToPlainLinear(text, 0.0f, 100.0f);
    }
    else
    {
      auto plainLinear = TextToPlainLinear(text, plainMin, plainMax);
      if (!plainLinear)
        return {};
      return PlainLinearToNormalized(plainLinear.value());
    }

  std::optional<int> discrete = {};
  if (list.size() != 0)
    discrete = TextToDiscreteList(text, io, list);
  else if (valueCount == 2)
    discrete = TextToDiscreteBool(text);
  else
    discrete = TextToDiscreteInt(text, valueCount);

  if (!discrete)
    return {};
  return DiscreteToNormalized(discrete.value());
}