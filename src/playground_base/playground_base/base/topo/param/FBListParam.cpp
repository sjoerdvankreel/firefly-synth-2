#include <playground_base/base/topo/param/FBListParam.hpp>

#include <cassert>

using namespace juce; 

int
FBListParamNonRealTime::ValueCount() const
{
  return (int)items.size();
}

double 
FBListParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return FBListParamRealTime::NormalizedToPlain((float)normalized);
}

double
FBListParamNonRealTime::PlainToNormalized(double plain) const
{
  int count = (int)items.size();
  return std::clamp(plain / (count - 1.0), 0.0, 1.0);
}

PopupMenu
FBListParamNonRealTime::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < ValueCount(); i++)
    result.addItem(i + 1, PlainToText(FBValueTextDisplay::Text, i));
  return result;
}

std::optional<double>
FBListParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == (display == FBValueTextDisplay::IO ? items[i].id : items[i].text))
      return { i };
  return {};
}

std::string
FBListParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const
{
  int plainDiscrete = (int)std::round(plain);
  switch (display)
  {
  case FBValueTextDisplay::IO: return items[plainDiscrete].id;
  case FBValueTextDisplay::Text: return items[plainDiscrete].text;
  case FBValueTextDisplay::Tooltip: return items[plainDiscrete].tooltip.empty()? items[plainDiscrete].text: items[plainDiscrete].tooltip;
  default: assert(false); return {};
  }
}