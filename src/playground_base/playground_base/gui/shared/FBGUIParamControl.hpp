#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBGUIParamComponent.hpp>

class FBGUIParamControl:
public FBGUIParamComponent
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIParamControl);
  FBGUIParamControl(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
  virtual void SetValueNormalizedFromPlug(float normalized) = 0;
};