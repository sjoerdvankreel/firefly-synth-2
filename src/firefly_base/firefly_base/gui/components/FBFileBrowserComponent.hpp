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
public juce::Component
{
  FBPlugGUI* const _plugGUI;
  std::function<void(std::string const&)> _onSelect;

  std::unique_ptr<FBGridComponent> _grid = {};
  std::unique_ptr<FBMarginComponent> _margin = {};
  std::unique_ptr<FBAutoSizeButton> _okButton = {};
  std::unique_ptr<FBAutoSizeButton> _cancelButton = {};
  std::unique_ptr<juce::WildcardFileFilter> _filter = {};
  std::unique_ptr<juce::FileBrowserComponent> _browser = {};

  void Hide();

public:
  ~FBFileBrowserComponent();
  FBFileBrowserComponent(
    FBPlugGUI* plugGUI, bool isSave, 
    std::string extension, std::string filterName, 
    std::function<void(std::string const&)> onSelect);
  
  void Show();
  void resized() override;
};