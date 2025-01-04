#include <playground_base/base/topo/param/FBListParam.hpp>

float
FBListParam::PlainToNormalized(int plain) const
{
  int count = (int)list.size();
  return std::clamp(plain / (count - 1.0f), 0.0f, 1.0f);
}

std::string
FBListParam::PlainToText(bool io, int plain) const
{
  return io ? list[plain].id : list[plain].text;
}

std::optional<int>
FBListParam::TextToPlain(bool io, std::string const& text) const
{
  for (int i = 0; i < list.size(); i++)
    if (text == (io ? list[i].id : list[i].text))
      return { i };
  return {};
}