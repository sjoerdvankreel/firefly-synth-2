#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>

class FBPlugGUI;
struct FBTheme;
struct FBColorScheme;
struct FBRuntimeParam;
struct FBRuntimeGUIParam;

class FBParamComponent:
public FBParamsDependent,
public IFBThemingComponent
{
  bool const _isThemed;

protected:
  FBRuntimeParam const* const _param;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamComponent);
  FBRuntimeParam const* Param() const { return _param; }
  FBColorScheme const* GetScheme(FBTheme const& theme) const override;
  FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed);
};

class FBGUIParamComponent:
public FBParamsDependent,
public IFBThemingComponent
{
  bool const _isThemed;

protected:
  FBRuntimeGUIParam const* const _param;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIParamComponent);
  FBRuntimeGUIParam const* Param() const { return _param; }
  FBColorScheme const* GetScheme(FBTheme const& theme) const override;
  FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed);
};

class FBParamControl:
public FBParamComponent
{
  bool _isFlashDisabling = false;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamControl);
  FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed);
  
  void StartFlashDisabling();
  bool IsHighlightTweaked() const;
  bool IsFlashDisabling() const { return _isFlashDisabling; }
  virtual void SetValueNormalizedFromHost(double normalized) = 0;
};

class FBGUIParamControl:
public FBGUIParamComponent
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIParamControl);
  FBGUIParamControl(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed);
  virtual void SetValueNormalized(double normalized) = 0;
};