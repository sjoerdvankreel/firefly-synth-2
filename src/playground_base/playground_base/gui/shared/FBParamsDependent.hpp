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

class FBParamsDependent
{
protected:
  FBPlugGUI* const _plugGUI;
  juce::Component* _initialParent = {};

  void ParentHierarchyChanged();

private:
  std::unique_ptr<FBParamsDependentDependency> _visibleWhenGUI;
  std::unique_ptr<FBParamsDependentDependency> _enabledWhenGUI;
  std::unique_ptr<FBParamsDependentDependency> _visibleWhenAudio;
  std::unique_ptr<FBParamsDependentDependency> _enabledWhenAudio;

public:
  virtual ~FBParamsDependent();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamsDependent);
  FBParamsDependent(
    FBPlugGUI* plugGUI, 
    FBTopoIndices const& moduleIndices,
    FBParamsDependencies const& dependencies);

  void DependenciesChanged(bool visible);
  std::vector<int> const& RuntimeDependencies(bool audio, bool visible) const;
};