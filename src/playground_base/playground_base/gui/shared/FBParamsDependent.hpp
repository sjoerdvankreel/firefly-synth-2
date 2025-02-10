#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <memory>

class FBPlugGUI;
struct FBTopoIndices;
struct FBParamsDependency;
struct FBParamsDependencies;
struct FBParamsDependentDependency;

class FBParamsDependent
{
protected:
  FBPlugGUI* const _plugGUI;

private:
  std::unique_ptr<FBParamsDependentDependency> _visible;
  std::unique_ptr<FBParamsDependentDependency> _enabled;

public:
  virtual ~FBParamsDependent();
  void DependenciesChanged(bool visible);
  std::vector<int> const& RuntimeDependencies(bool visible) const;
  
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamsDependent);
  FBParamsDependent(
    FBPlugGUI* plugGUI, FBTopoIndices const& moduleIndices,
    int staticParamSlot, FBParamsDependencies const& dependencies);
};