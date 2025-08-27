#include <firefly_base/base/topo/static/FBParamsDependencies.hpp>

void
FBParamsDependency::WhenSlots(
  std::vector<FBTopoIndices> const& staticParamIndices_,
  FBSlotParamsDependencyEvaluator const& evaluate_)
{
  evaluate = evaluate_;
  staticParamIndices = staticParamIndices_;
}

void
FBParamsDependency::WhenSimple(
  std::vector<int> const& staticParamIndices_,
  FBSimpleParamsDependencyEvaluator const& evaluateSimple)
{
  evaluate = [evaluateSimple](auto const&, auto const& vals) { return evaluateSimple(vals); };
  staticParamIndices.clear();
  for (int i = 0; i < staticParamIndices_.size(); i++)
    staticParamIndices.push_back({ staticParamIndices_[i], -1 });
}