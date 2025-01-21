#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBParamsDependency.hpp>

#include <vector>

class FBPlugGUI;
struct FBTopoIndices;

class FBParamsDependent
{
private:
  std::vector<int> _evaluations;
  FBParamsDependency _dependency;
  std::vector<int> const _runtimeDependencies;

protected:
  FBPlugGUI* const _plugGUI;
  virtual void DependenciesChanged(bool outcome) = 0;

public:
  FBParamsDependent(
    FBPlugGUI* plugGUI, FBTopoIndices const& moduleIndices,
    int staticParamSlot, FBParamsDependency const& dependency);

  void DependenciesChanged();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamsDependent);
  std::vector<int> const& RuntimeDependencies() const { return _runtimeDependencies; }
};