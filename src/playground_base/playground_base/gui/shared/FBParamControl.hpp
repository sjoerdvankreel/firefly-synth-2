#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBEnabledTarget.hpp>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamControl:
public FBEnabledTarget
{
protected:
  FBRuntimeParam const* const _param;

public:
  FBRuntimeParam const* Param() const { return _param; }
  bool Evaluate(std::vector<int> const& vals) const override;
  virtual void SetValueNormalizedFromHost(float normalized) = 0;

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamControl);
  FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};