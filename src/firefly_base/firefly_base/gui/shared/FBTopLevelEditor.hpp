#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <functional>

class FBPlugGUI;

struct FBTopLevelEditorParamsDetail final
{
  juce::DialogWindow* dialog = {};
  juce::Component* contentGrid = {};
  juce::Component* contentComponent = {};
};

struct FBTopLevelEditorParams final
{
  int w = -1;
  int h = -1;
  std::string title = {};
  std::string iconFile = {};
  juce::Component* header = {};
  juce::Component* content = {};
  std::function<void()> init = {};
  FBTopLevelEditorParamsDetail detail = {};
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