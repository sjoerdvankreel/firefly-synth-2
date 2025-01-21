#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBParamsDependency.hpp>

#include <vector>

class FBPlugGUI;

class FBParamsDependent
{
protected:
  FBPlugGUI* const _plugGUI;

private:
  std::vector<int> _evaluations;
  FBParamsDependency _dependency;
  std::vector<int> const _runtimeDependencies;

public:
  void DependenciesChanged();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamsDependent);
  FBParamsDependent(FBPlugGUI* plugGUI, FBParamsDependency const& dependency);
  std::vector<int> const& RuntimeDependencies() const { return _runtimeDependencies; }
};