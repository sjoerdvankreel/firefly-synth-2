#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>

struct FBHostContextMenuItem
{
  std::string name = {};
  bool checked = false;
  bool enabled = false;
  bool separator = false;
  bool subMenuEnd = false;
  bool subMenuStart = false;
};

std::unique_ptr<juce::PopupMenu>
FBMakeHostContextMenu(std::vector<FBHostContextMenuItem> const& items);