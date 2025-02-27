#include <playground_base/base/topo/param/FBListParam.hpp>

#include <cassert>

using namespace juce; 

int 
FBListParamNonRealTime::ValueCount() const
{
  return (int)items.size();
}

int 
FBListParamNonRealTime::NormalizedToPlain(float normalized) const
{
  return FBListParamRealTime::NormalizedToPlain(normalized);
}

float
FBListParamNonRealTime::PlainToNormalized(int plain) const
{
  int count = (int)items.size();
  return std::clamp(plain / (count - 1.0f), 0.0f, 1.0f);
}

PopupMenu
FBListParamNonRealTime::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < ValueCount(); i++)
    result.addItem(i + 1, PlainToText(FBValueTextDisplay::Text, i));
  return result;
}

std::optional<int>
FBListParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == (display == FBValueTextDisplay::IO ? items[i].id : items[i].text))
      return { i };
  return {};
}

std::string
FBListParamNonRealTime::PlainToText(FBValueTextDisplay display, int plain) const
{
  switch (display)
  {
  case FBValueTextDisplay::IO: return items[plain].id;
  case FBValueTextDisplay::Text: return items[plain].text;
  case FBValueTextDisplay::Tooltip: return items[plain].tooltip.empty()? items[plain].text: items[plain].tooltip;
  default: assert(false); return {};
  }
}