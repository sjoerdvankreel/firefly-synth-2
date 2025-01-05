#pragma once

#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

class FFPlugGUI :
public FBPlugGUI
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(IFBHostGUIContext* hostContext);

  int MinWidth() const override;
  int MaxWidth() const override;
  int DefaultWidth() const override;
  void SetParamNormalized(int index, float normalized) override;
};