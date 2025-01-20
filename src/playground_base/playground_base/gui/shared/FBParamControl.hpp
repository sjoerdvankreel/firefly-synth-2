#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBEnabledTarget.hpp>

struct FBRuntimeTopo;
struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamControl:
public FBEnabledTarget
{
protected:
  FBRuntimeParam const* const _param;

public:
  FBParamControl(
    FBRuntimeTopo const* topo,
    FBRuntimeParam const* param,
    IFBHostGUIContext* hostContext);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamControl);

  virtual void SetValueNormalized(float normalized) = 0;
  FBRuntimeParam const* Param() const { return _param; }
  bool Evaluate(std::vector<int> const& vals) const override;
};