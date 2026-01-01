#pragma once

#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <memory>
#include <functional>

class FBPlugGUI;
class FBAutoSizeButton;

class FBFileBrowserComponent:
public juce::Component,
public juce::FileBrowserListener
{
  FBPlugGUI* const _plugGUI;
  std::string const _extension;
  std::function<void(juce::File&)> _onSelect;

  std::unique_ptr<FBGridComponent> _grid = {};
  std::unique_ptr<FBMarginComponent> _margin = {};
  std::unique_ptr<FBAutoSizeButton> _okButton = {};
  std::unique_ptr<FBAutoSizeButton> _cancelButton = {};
  std::unique_ptr<juce::WildcardFileFilter> _filter = {};
  std::unique_ptr<juce::FileBrowserComponent> _browser = {};

public:
  ~FBFileBrowserComponent();
  FBFileBrowserComponent(
    FBPlugGUI* plugGUI, bool isSave, 
    std::string extension, std::string filterName, 
    std::function<void(juce::File&)> onSelect);
  
  void Show();
  void Hide();
  
  void resized() override;
  void selectionChanged() override {};
  void fileDoubleClicked(const juce::File& /*file*/) override;
  void browserRootChanged(const juce::File& /*newRoot*/) override {};
  void fileClicked(const juce::File& /*file*/, const juce::MouseEvent& /*e*/) override {};
};