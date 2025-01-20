#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBRuntimeTopo;
class IFBHostGUIContext;

// TODO also visible
class FBEnabledTarget
{
  std::vector<int> _evaluateValues;
  std::vector<int> const _evaluateWhen;

protected:
  FBRuntimeTopo const* const _topo;
  IFBHostGUIContext* const _hostContext;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBEnabledTarget);
  FBEnabledTarget(
    FBRuntimeTopo const* topo, 
    IFBHostGUIContext* hostContext,
    std::vector<int> const& evaluateWhen);

  bool Evaluate();
  virtual bool Evaluate(std::vector<int> const& vals) const = 0;
  std::vector<int> const& EvaluateWhen() const { return _evaluateWhen; }
};