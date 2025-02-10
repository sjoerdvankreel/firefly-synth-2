#include <playground_base/base/topo/param/FBTimeSigParam.hpp>

#include <cassert>

using namespace juce;

std::string
FBTimeSigParam::PlainToText(int plain) const
{
  return items[plain].ToString();
}

PopupMenu
FBTimeSigParam::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < ValueCount(); i++)
    result.addItem(i + 1, PlainToText(i));
  return result;
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