#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBEnabledTarget.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

FBEnabledTarget::
FBEnabledTarget(
  FBPlugGUI* plugGUI, 
  std::vector<int> const& evaluateWhen):
_plugGUI(plugGUI),
_evaluateValues(),
_evaluateWhen(evaluateWhen) {}

bool
FBEnabledTarget::Evaluate()
{
  _evaluateValues.clear();
  for (int i = 0; i < _evaluateWhen.size(); i++)
    _evaluateValues.push_back(
      _plugGUI->Topo()->params[_evaluateWhen[i]].static_.NormalizedToAnyDiscreteSlow(
        _plugGUI->HostContext()->GetParamNormalized(_evaluateWhen[i])));
  return Evaluate(_evaluateValues);
}