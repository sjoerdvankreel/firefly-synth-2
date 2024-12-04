#include <playground_base/base/topo/FBStaticParam.hpp>

#include <cassert>

static std::optional<int>
TextToDiscreteBool(std::string const& text)
{
  if (text != "On" && text != "Off")
    return {};
  return { text == "On" ? 1 : 0 };
}

static std::optional<double>
TextToNormalizedContinuous(std::string const& text)
{
  char* end;
  double result = std::strtod(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < 0.0 || result > 1.0)
    return {};
  return { result };
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

std::string
FBStaticParam::NormalizedToText(bool io, double normalized) const
{
  assert(valueCount != 1);
  assert(list.size() == 0 || list.size() == valueCount);

  if (valueCount == 0)
    return std::to_string(normalized);

  int discrete = NormalizedToDiscrete(normalized);
  if (list.size() != 0)
    return io ? list[discrete].id : list[discrete].text;
  if (valueCount == 2)
    return discrete == 0 ? "Off" : "On";
  return std::to_string(discrete);
}

std::optional<double>
FBStaticParam::TextToNormalized(std::string const& text, bool io) const
{
  assert(valueCount != 1);
  assert(list.size() == 0 || list.size() == valueCount);

  if (valueCount == 0)
    return TextToNormalizedContinuous(text);

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