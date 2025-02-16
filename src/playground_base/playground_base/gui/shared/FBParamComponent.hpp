#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

class FBPlugGUI;
struct FBRuntimeAudioParam;

class FBParamComponent:
public FBParamsDependent
{
protected:
  FBRuntimeAudioParam const* const _param;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamComponent);
  FBRuntimeAudioParam const* Param() const { return _param; }
  FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeAudioParam const* param);
};