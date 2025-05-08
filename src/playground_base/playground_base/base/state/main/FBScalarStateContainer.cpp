#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>

#include <set>
#include <cassert>

FBScalarStateContainer::
FBScalarStateContainer(FBRuntimeTopo const& topo):
_params(),
_rawState(topo.static_.allocRawScalarState()),
_freeRawState(topo.static_.freeRawScalarState)
{
  for (int p = 0; p < topo.audio.params.size(); p++)
    _params.push_back(
      topo.audio.params[p].static_.scalarAddr(
        topo.audio.params[p].topoIndices.module.slot,
        topo.audio.params[p].topoIndices.param.slot, _rawState));
  for (int p = 0; p < _params.size(); p++)
    *_params[p] = topo.audio.params[p].static_.DefaultNormalizedByText();
#ifndef NDEBUG
  std::set<double*> uniquePtrs(_params.begin(), _params.end());
  assert(uniquePtrs.size() == _params.size());
#endif
}

void
FBScalarStateContainer::CopyFrom(FBProcStateContainer const& proc)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = proc.Params()[p].Value();
}

void 
FBScalarStateContainer::CopyFrom(FBScalarStateContainer const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = *scalar.Params()[p];
}