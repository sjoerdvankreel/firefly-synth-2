#include <playground_base/base/topo/param/FBListParam.hpp>

#include <cassert>

using namespace juce;

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

PopupMenu
FBListParam::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < ValueCount(); i++)
    result.addItem(i + 1, PlainToText(FBValueTextDisplay::Text, i));
  return result;
}

std::string
FBListParam::PlainToText(FBValueTextDisplay display, int plain) const
{
  switch (display)
  {
  case FBValueTextDisplay::IO: return items[plain].id;
  case FBValueTextDisplay::Text: return items[plain].text;
  case FBValueTextDisplay::Tooltip: return items[plain].tooltip.empty()? items[plain].text: items[plain].tooltip;
  default: assert(false); return {};
  }
}