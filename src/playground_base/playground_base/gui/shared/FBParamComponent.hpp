#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBEnabledTarget.hpp>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamComponent:
public FBEnabledTarget
{
protected:
  FBRuntimeParam const* const _param;

public:
  FBRuntimeParam const* Param() const { return _param; }
  bool Evaluate(std::vector<int> const& vals) const override;

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamComponent);
  FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};