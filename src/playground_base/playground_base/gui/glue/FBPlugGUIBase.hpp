#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUIBase:
public juce::Component,
public IFBPlugGUI
{
  float _scale = 1.0f;

protected:
  FBPlugGUIBase();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIBase);

protected:
  virtual IFBParamControl* 
  GetParamControlForIndex(int index) = 0;

public:
  void paint(juce::Graphics& g) override;

  int GetScaledWidth() const override;
  int GetScaledHeight() const override;
  int GetMinScaledWidth() const override;
  int GetMaxScaledWidth() const override;
  void SetScaledSize(int w, int h) override;
  int GetDefaultUnscaledHeight() const override;
  void SetContentScaleFactor(float scale) override;

  void RemoveFromDesktop() override;
  void AddToDesktop(void* parent) override;
  void SetVisible(bool visible) override;
  void SetParamNormalized(int index, float normalized) override;
};