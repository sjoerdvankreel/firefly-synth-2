#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class FBPlugGUI;
struct FBTheme;
struct FBColorScheme;
struct FBRuntimeTopo;

class IFBThemingComponent
{
public:
  virtual ~IFBThemingComponent() {}
  virtual FBColorScheme const* GetScheme(FBTheme const& theme) const = 0;
};

// Tags another component with an id to track it in the theming.
class FBThemedComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize,
public IFBThemingComponent
{
  int _componentId;
  juce::Component* _content;
  FBRuntimeTopo const* const _topo;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;

  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBColorScheme const* GetScheme(FBTheme const& theme) const override;

  int ComponentId() const { return _componentId; }
  FBThemedComponent(FBRuntimeTopo const* topo, int componentId, juce::Component* content);
};

// Tags another component with module topo.
// Useful for finding ancestors when applying theme colors.
class FBModuleComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize,
public IFBThemingComponent
{
  int _moduleSlot = -1;
  int _moduleIndex = -1;
  juce::Component* _content = nullptr;
  FBRuntimeTopo const* const _topo;
  std::function<bool(FBTheme const&)> _followModule = {};

public:
  int ModuleSlot() const { return _moduleSlot; }
  int ModuleIndex() const { return _moduleIndex; }
  FBColorScheme const* GetScheme(FBTheme const& theme) const override;

  void resized() override;
  int FixedHeight() const override;
  int FixedWidth(int height) const override;

  FBModuleComponent(FBRuntimeTopo const* topo);
  FBModuleComponent(FBRuntimeTopo const* topo, int moduleIndex, int moduleSlot, juce::Component* content);
  FBModuleComponent(FBRuntimeTopo const* topo, int moduleIndex, int moduleSlot, std::function<bool(FBTheme const&)> const& followModule, juce::Component* content);
  void SetModuleContent(int moduleIndex, int moduleSlot, juce::Component* content);
};