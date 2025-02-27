#include <playground_base/base/topo/param/FBBarsParam.hpp>

#include <set>
#include <cassert>

using namespace juce;

int 
FBBarsParamNonRealTime::ValueCount() const
{
  return (int)items.size();
}

float
FBBarsParamNonRealTime::PlainToNormalized(int plain) const 
{
  int count = (int)items.size();
  return std::clamp(plain / (count - 1.0f), 0.0f, 1.0f);
}

int
FBBarsParamNonRealTime::NormalizedToPlain(float normalized) const 
{
  return FBBarsParamRealTime::NormalizedToPlain(normalized);
}

std::string
FBBarsParamNonRealTime::PlainToText(FBValueTextDisplay display, int plain) const 
{
  return items[plain].ToString();
}

std::optional<int>
FBBarsParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == items[i].ToString())
      return { i };
  return {};
}

PopupMenu
FBBarsParamNonRealTime::MakePopupMenu() const
{
  int k = 0;
  PopupMenu result;
  PopupMenu subMenu;
  for (int i = 0; i < items.size(); i++)
  {
    subMenu.addItem(i + 1, PlainToText(FBValueTextDisplay::Text, i));
    if (i == items.size() - 1 || items[i].num != items[i + 1].num)
    {
      result.addSubMenu(std::to_string(items[i].num), subMenu);
      subMenu = {};
    }
  }
  return result;
}

std::vector<FBBarsItem>
FBMakeBarsItems(bool withZero, FBBarsItem min, FBBarsItem max)
{
  static std::vector<int> const
    BarSteps({
      1, 2, 3, 4, 5, 6, 7, 8,
      9, 10, 11, 12, 13, 14, 15, 16,
      20, 24, 32, 48, 64, 96, 128 });

  std::set<FBBarsItem> equals;
  std::vector<FBBarsItem> result;

  if (withZero)
    result.push_back({ 0, -1 });
  for (int i = 0; i < BarSteps.size(); i++)
    for (int j = 0; j < BarSteps.size(); j++)
    {
      FBBarsItem item = { BarSteps[i], BarSteps[j] };
      if (min <= item && item <= max)
        if(equals.insert(item).second)
          result.push_back(item);
    }
  return result;
}