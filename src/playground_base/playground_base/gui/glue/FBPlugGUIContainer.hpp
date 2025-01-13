#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class FBPlugGUI;
class IFBHostGUIContext;

struct FBRuntimeTopo;

class FBPlugGUIContainer final:
public juce::Component,
public FBPlugGUIContext
{
  std::unique_ptr<FBPlugGUI> _gui;

protected:
  void RequestRescale(float combinedScale) override;

public:
  ~FBPlugGUIContainer();
  FBPlugGUIContainer(
    FBRuntimeTopo const* topo,
    IFBHostGUIContext* context);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIContainer);

  void paint(juce::Graphics& g) override;
  void RemoveFromDesktop() override;
  void AddToDesktop(void* parent) override;
  void SetVisible(bool visible) override;
  void SetParamNormalized(int index, float normalized) override;
};