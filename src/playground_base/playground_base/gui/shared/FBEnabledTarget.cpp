#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/shared/FBEnabledTarget.hpp>

FBEnabledTarget::
FBEnabledTarget(
  FBRuntimeTopo const* topo, 
  IFBHostGUIContext* hostContext,
  std::vector<int> const& evaluateWhen):
_evaluateValues(),
_evaluateWhen(evaluateWhen),
_topo(topo),
_hostContext(hostContext) {}

bool
FBEnabledTarget::Evaluate()
{
  _evaluateValues.clear();
  for (int i = 0; i < _evaluateWhen.size(); i++)
    _evaluateValues.push_back(
      _topo->params[_evaluateWhen[i]].static_.NormalizedToAnyDiscreteSlow(
        _hostContext->GetParamNormalized(_evaluateWhen[i])));
  return Evaluate(_evaluateValues);
}