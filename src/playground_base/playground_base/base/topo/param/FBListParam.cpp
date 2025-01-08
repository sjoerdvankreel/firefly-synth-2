#include <playground_base/base/topo/param/FBListParam.hpp>

double
FBListParam::PlainToNormalized(int plain) const
{
  int count = (int)items.size();
  return std::clamp(plain / (count - 1.0), 0.0, 1.0);
}

std::string
FBListParam::PlainToText(bool io, int plain) const
{
  return io ? items[plain].id : items[plain].text;
}

std::optional<int>
FBListParam::TextToPlain(bool io, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == (io ? items[i].id : items[i].text))
      return { i };
  return {};
}