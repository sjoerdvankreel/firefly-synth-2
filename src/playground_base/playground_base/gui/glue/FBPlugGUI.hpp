#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/components/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI:
public juce::Component  
{
protected:
  FBPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUI);

protected:
  virtual IFBParamControl* GetParamControlForIndex(int index) = 0;

public:
  void paint(juce::Graphics& g) override;

  int DefaultHeight() const;
  void SetParamNormalized(int index, float normalized);

  virtual int MinWidth() const = 0;
  virtual int MaxWidth() const = 0;
  virtual int DefaultWidth() const = 0;
  virtual int AspectRatioWidth() const = 0;
  virtual int AspectRatioHeight() const = 0;
  virtual void SetContentScaleFactor(float scale) = 0;
};