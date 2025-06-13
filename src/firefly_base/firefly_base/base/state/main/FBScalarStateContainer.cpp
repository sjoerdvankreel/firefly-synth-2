#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <set>

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
    *_params[p] = topo.audio.params[p].DefaultNormalizedByText();
#ifndef NDEBUG
  std::set<double*> uniquePtrs(_params.begin(), _params.end());
  FB_ASSERT(uniquePtrs.size() == _params.size());
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