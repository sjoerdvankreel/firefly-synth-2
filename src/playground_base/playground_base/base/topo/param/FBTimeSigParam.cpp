#include <playground_base/base/topo/param/FBTimeSigParam.hpp>

#include <cassert>

using namespace juce;

static std::vector<int>
ItemSteps({
  1, 2, 3, 4, 5, 6, 7, 8,
  9, 10, 11, 12, 13, 14, 15, 16,
  20, 24, 32, 48, 64, 96, 128 });

std::string
FBTimeSigParam::PlainToText(int plain) const
{
  return items[plain].ToString();
}

float
FBTimeSigParam::PlainToNormalized(int plain) const
{
  int count = (int)items.size();
  return std::clamp(plain / (count - 1.0f), 0.0f, 1.0f);
}

std::optional<int>
FBTimeSigParam::TextToPlain(std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == items[i].ToString())
      return { i };
  return {};
}

std::vector<FBTimeSigItem>
FBMakeTimeSigItems(FBTimeSigItem min, FBTimeSigItem max)
{
  std::vector<FBTimeSigItem> result;
  for (int i = 0; i < ItemSteps.size(); i++)
    for (int j = 0; j < ItemSteps.size(); j++)
    {
      FBTimeSigItem item = { ItemSteps[i], ItemSteps[j] };
      if(min <= item && item <= max)
        result.push_back(item);
    }
  return result;
}

PopupMenu
FBTimeSigParam::MakePopupMenu() const
{
  int k = 0;
  PopupMenu result;
  PopupMenu subMenu;
  for (int i = 0; i < items.size(); i++)
  {
    subMenu.addItem(i + 1, PlainToText(i));
    if (i == items.size() - 1 || items[i].num != items[i + 1].num)
    {
      result.addSubMenu(std::to_string(items[i].num), subMenu);
      subMenu = {};
    }
  }
  return result;
}