#include <playground_base/base/topo/param/FBTimeSigParam.hpp>

#include <cassert>

using namespace juce;

static std::vector<int>
DefaultSteps({
  1, 2, 3, 4, 5, 6, 7, 8,
  9, 10, 11, 12, 13, 14, 15, 16,
  20, 24, 32, 48, 64, 96, 128 });

std::string
FBTimeSigParam::PlainToText(int plain) const
{
  return DefaultItems()[plain].ToString();
}

float
FBTimeSigParam::PlainToNormalized(int plain) const
{
  int count = (int)DefaultItems().size();
  return std::clamp(plain / (count - 1.0f), 0.0f, 1.0f);
}

std::optional<int>
FBTimeSigParam::TextToPlain(std::string const& text) const
{
  for (int i = 0; i < DefaultItems().size(); i++)
    if (text == DefaultItems()[i].ToString())
      return { i };
  return {};
}

std::vector<FBTimeSigItem> const&
FBTimeSigParam::DefaultItems()
{
  thread_local std::vector<FBTimeSigItem> result;
  if (!result.empty())
    return result;
  for (int i = 0; i < DefaultSteps.size(); i++)
    for (int j = 0; j < DefaultSteps.size(); j++)
      result.push_back({ DefaultSteps[i], DefaultSteps[j] });
  return result;
}

PopupMenu
FBTimeSigParam::MakePopupMenu() const
{
  int k = 0;
  PopupMenu result;
  for (int i = 0; i < DefaultSteps.size(); i++)
  {
    PopupMenu subMenu;
    for (int j = 0; j < DefaultSteps.size(); j++, k++)
      subMenu.addItem(k + 1, PlainToText(k));
    result.addSubMenu(std::to_string(DefaultSteps[i]), subMenu);
  }
  return result;
}