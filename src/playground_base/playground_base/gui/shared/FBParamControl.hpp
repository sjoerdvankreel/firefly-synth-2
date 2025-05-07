#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>

class FBParamControl:
public FBParamComponent
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamControl);
  FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
  virtual void SetValueNormalizedFromHost(double normalized) = 0;
};