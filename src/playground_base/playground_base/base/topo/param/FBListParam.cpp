#include <playground_base/base/topo/param/FBListParam.hpp>

#include <cassert>

float
FBListParam::PlainToNormalized(int plain) const
{
  int count = (int)items.size();
  return std::clamp(plain / (count - 1.0f), 0.0f, 1.0f);
}

std::optional<int>
FBListParam::TextToPlain(bool io, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == (io ? items[i].id : items[i].text))
      return { i };
  return {};
}

std::string
FBListParam::PlainToText(FBTextDisplay display, int plain) const
{
  switch (display)
  {
  case FBTextDisplay::IO: return items[plain].id;
  case FBTextDisplay::Text: return items[plain].text;
  case FBTextDisplay::Tooltip: return items[plain].tooltip.empty()? items[plain].text: items[plain].tooltip;
  default: assert(false); return {};
  }
}