#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBPlugGUI;

struct FBTopLevelEditorParams final
{
  int w = -1;
  int h = -1;
  std::string title = {};
  std::string iconFile = {};
  juce::Component* content = {};
  juce::DialogWindow* dialog = {};
};

class FBTopLevelEditor final:
public juce::DialogWindow
{
  int _id;
  FBPlugGUI* const _plugGUI;
public:
  void closeButtonPressed() override;
  FBTopLevelEditor(FBPlugGUI* plugGUI, int id, DialogWindow::LaunchOptions& options);
};