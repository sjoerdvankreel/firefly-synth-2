#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

class FBPlugGUI;
struct FBRuntimeGUIParam;

class FBGUIParamComponent:
public FBParamsDependent
{
protected:
  FBRuntimeGUIParam const* const _param;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIParamComponent);
  FBRuntimeGUIParam const* Param() const { return _param; }
  FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
};