#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGUIStateContainer.hpp>

#include <set>
#include <cassert>

FBGUIStateContainer::
FBGUIStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.state.allocRawGUIState()),
_params(),
_freeRawState(topo.static_.state.freeRawGUIState)
{
  for (int p = 0; p < topo.gui.params.size(); p++)
    _params.push_back(
      topo.gui.params[p].static_.addrSelector(
        topo.gui.params[p].topoIndices.module.slot,
        topo.gui.params[p].topoIndices.param.slot, _rawState));
  for (int p = 0; p < _params.size(); p++)
    *_params[p] = topo.gui.params[p].static_.DefaultNormalizedByText();
#ifndef NDEBUG
  std::set<float*> uniquePtrs(_params.begin(), _params.end());
  assert(uniquePtrs.size() == _params.size());
#endif
}

void 
FBGUIStateContainer::CopyFrom(FBGUIStateContainer const& gui)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = *gui.Params()[p];
}