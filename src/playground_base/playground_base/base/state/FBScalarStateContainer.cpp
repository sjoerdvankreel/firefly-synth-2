#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <set>
#include <cassert>

FBScalarStateContainer::
FBScalarStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.allocRawScalarState()),
_params(),
_freeRawState(topo.static_.freeRawScalarState)
{
  for (int p = 0; p < topo.params.size(); p++)
    _params.push_back(topo.params[p].static_.scalarAddr(
      topo.params[p].staticModuleSlot, topo.params[p].staticSlot, _rawState));
#ifndef NDEBUG
  std::set<float*> uniquePtrs(_params.begin(), _params.end());
  assert(uniquePtrs.size() == _params.size());
#endif
  for (int p = 0; p < _params.size(); p++)
    *_params[p] = topo.params[p].static_.DefaultNormalizedByText();
}

void
FBScalarStateContainer::CopyFrom(FBProcStatePtrs const& proc)
{
  for (int p = 0; p < Params().size(); p++)
  {
    auto val = proc.Params()[p].Value();
    *Params()[p] = val;
    auto val2 = *Params()[p];
    (void)val2;
  }
}

void 
FBScalarStateContainer::CopyFrom(FBScalarStateContainer const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = *scalar.Params()[p];
}