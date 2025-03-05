#include <playground_base/base/topo/param/FBListParam.hpp>

#include <cassert>

using namespace juce;

bool 
FBListParamNonRealTime::IsItems() const
{
  return true;
}

bool 
FBListParamNonRealTime::IsStepped() const
{
  return true;
}

int 
FBListParamNonRealTime::ValueCount() const 
{
  return static_cast<int>(items.size());
}

double 
FBListParamNonRealTime::PlainToNormalized(double plain) const 
{
  return std::clamp(plain / (ValueCount() - 1.0), 0.0, 1.0);
}

double
FBListParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  return std::clamp(static_cast<int>(normalized * ValueCount()), 0, ValueCount() - 1);
}

PopupMenu
FBListParamNonRealTime::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < ValueCount(); i++)
    result.addItem(i + 1, PlainToText(FBTextDisplay::Text, i));
  return result;
}

std::optional<double>
FBListParamNonRealTime::TextToPlain(FBTextDisplay display, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == ((display == FBTextDisplay::IO) ? items[i].id : items[i].text))
      return { i };
  return {};
}

std::string
FBListParamNonRealTime::PlainToText(FBTextDisplay display, double plain) const
{
  int discrete = static_cast<int>(std::round(plain));
  switch (display)
  {
  case FBTextDisplay::IO: return items[discrete].id;
  case FBTextDisplay::Text: return items[discrete].text;
  case FBTextDisplay::Tooltip: return items[discrete].tooltip.empty() ? items[discrete].text : items[discrete].tooltip;
  default: assert(false); return {};
  }
}