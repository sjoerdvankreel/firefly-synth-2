#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>

#include <set>
#include <cassert>

FBGUIStateContainer::
FBGUIStateContainer(FBRuntimeTopo const& topo):
_params(),
_rawState(topo.static_.allocRawGUIState()),
_freeRawState(topo.static_.freeRawGUIState),
_special(topo.static_.specialGUISelector(topo.static_, _rawState))
{
  for (int p = 0; p < topo.gui.params.size(); p++)
    _params.push_back(
      topo.gui.params[p].static_.scalarAddr(
        topo.gui.params[p].topoIndices.module.slot,
        topo.gui.params[p].topoIndices.param.slot, _rawState));
  for (int p = 0; p < _params.size(); p++)
    *_params[p] = topo.gui.params[p].DefaultNormalizedByText();
#ifndef NDEBUG
  std::set<double*> uniquePtrs(_params.begin(), _params.end());
  FB_ASSERT(uniquePtrs.size() == _params.size());
#endif
}

void 
FBGUIStateContainer::CopyFrom(FBGUIStateContainer const& gui)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = *gui.Params()[p];
}