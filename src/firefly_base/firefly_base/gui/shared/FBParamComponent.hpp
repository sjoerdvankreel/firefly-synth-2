#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>

class FBPlugGUI;
struct FBRuntimeParam;
struct FBRuntimeGUIParam;

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

class FBParamControl:
public FBParamComponent
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamControl);
  FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
  virtual void SetValueNormalizedFromHost(double normalized) = 0;
};

class FBGUIParamControl:
public FBGUIParamComponent
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIParamControl);
  FBGUIParamControl(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
  virtual void SetValueNormalizedFromPlug(double normalized) = 0;
};