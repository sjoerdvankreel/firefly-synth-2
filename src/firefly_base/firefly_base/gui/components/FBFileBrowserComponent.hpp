#pragma once

#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>

#include <juce_core/juce_core.h>
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
  bool const _isSave;
  std::string const _extension;
  std::function<void(juce::File const&)> _onSelect;

  std::unique_ptr<FBGridComponent> _grid = {};
  std::unique_ptr<FBAutoSizeLabel> _title = {};
  std::unique_ptr<FBMarginComponent> _margin = {};
  std::unique_ptr<FBAutoSizeButton> _okButton = {};
  std::unique_ptr<FBAutoSizeButton> _cancelButton = {};
  std::unique_ptr<juce::WildcardFileFilter> _filter = {};
  std::unique_ptr<juce::FileBrowserComponent> _browser = {};

  void SelectFile(juce::File const& file);

public:
  ~FBFileBrowserComponent();
  FBFileBrowserComponent(
    FBPlugGUI* plugGUI, bool isSave, std::string const& title,
    std::string const& extension, std::string const& filterName, 
    std::function<void(juce::File const&)> onSelect);
  
  void Show();
  void Hide();
  
  void resized() override;
  void selectionChanged() override {}
  void fileDoubleClicked(const juce::File& /*file*/) override;
  void browserRootChanged(const juce::File& /*newRoot*/) override {};
  void fileClicked(const juce::File& /*file*/, const juce::MouseEvent& /*e*/) override {};
};