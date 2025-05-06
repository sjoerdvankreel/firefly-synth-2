#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class FBPlugGUI;
class FBHostGUIContext;

class FBPlugGUIContainer final:
public juce::Component,
public FBPlugGUIContext
{
  std::unique_ptr<FBPlugGUI> _gui;

protected:
  void RequestRescale(double combinedScale) override;

public:
  ~FBPlugGUIContainer();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIContainer);
  FBPlugGUIContainer(FBHostGUIContext* hostContext);

  void paint(juce::Graphics& g) override;

  void RemoveFromDesktop() override;
  void UpdateExchangeState() override;
  void SetVisible(bool visible) override;
  void AddToDesktop(void* parent) override;
  void SetAudioParamNormalizedFromHost(int index, double normalized) override;
};