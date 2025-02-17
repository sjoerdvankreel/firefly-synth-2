#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>
#include <memory>

class FBPlugGUI;
struct FBTopoIndices;
struct FBParamsDependency;
struct FBParamsDependencies;
struct FBParamsDependentDependency;

// TODO make it work with GUI-only params
class FBParamsDependent
{
protected:
  FBPlugGUI* const _plugGUI;
  juce::Component* _initialParent = {};

  void ParentHierarchyChanged();

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