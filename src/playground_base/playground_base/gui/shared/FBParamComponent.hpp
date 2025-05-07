#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamComponent:
public FBParamsDependent
{
protected:
  FBRuntimeParam const* const _param;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamComponent);
  FBRuntimeParam const* Param() const { return _param; }
  FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};