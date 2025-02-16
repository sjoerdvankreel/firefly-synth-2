#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>

class FBParamControl:
public FBParamComponent
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamControl);
  FBParamControl(FBPlugGUI* plugGUI, FBRuntimeAudioParam const* param);
  virtual void SetValueNormalizedFromHost(float normalized) = 0;
};