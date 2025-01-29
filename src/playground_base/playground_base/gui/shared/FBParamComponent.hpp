#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamComponent:
public FBParamsDependent
{
protected:
  void UpdateTooltip();
  FBRuntimeParam const* const _param;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamComponent);
  void DependenciesChanged(bool outcome) override;
  FBRuntimeParam const* Param() const { return _param; }
  FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};