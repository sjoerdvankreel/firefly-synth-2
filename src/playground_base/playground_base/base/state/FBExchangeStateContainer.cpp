#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>

#include <set>
#include <cassert>

FBExchangeStateContainer::
FBExchangeStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.state.allocRawExchangeState()),
_freeRawState(topo.static_.state.freeRawExchangeState)
{
  _voiceActive = topo.static_.state.voiceActiveExchangeAddr(_rawState);
  for (int p = 0; p < topo.params.size(); p++)
    if (topo.static_.modules[topo.params[p].topoIndices.module.index].voice)
      _params.push_back(FBExchangeParamState(topo.params[p].static_.voiceExchangeAddr(
          topo.params[p].topoIndices.module.slot, 
          topo.params[p].topoIndices.param.slot, _rawState)));
    else
      _params.push_back(FBExchangeParamState(topo.params[p].static_.globalExchangeAddr(
        topo.params[p].topoIndices.module.slot,
        topo.params[p].topoIndices.param.slot, _rawState)));
#ifndef NDEBUG
  std::set<void const*> uniquePtrs = {};
  for (int p = 0; p < _params.size(); p++)
    if (_params[p].IsGlobal())
      uniquePtrs.insert(_params[p].Global());
    else
      uniquePtrs.insert(&_params[p].Voice());
  assert(uniquePtrs.size() == _params.size());
#endif
}