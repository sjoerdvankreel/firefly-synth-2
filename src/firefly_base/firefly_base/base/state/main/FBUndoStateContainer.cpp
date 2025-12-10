#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

static void
SwapContextForItem(
  FBHostGUIContext* hostContext,
  FBUndoItem& item)
{
  FBScalarStateContainer temp(*hostContext->Topo());
  temp.CopyFrom(hostContext, false);
  item.state.CopyTo(hostContext, false);
  item.state.CopyFrom(hostContext->Topo(), temp, false);

  std::string tempName(hostContext->PatchName());
  hostContext->SetPatchName(item.patchName);
  item.patchName = tempName;
}

FBUndoStateContainer::
FBUndoStateContainer(FBHostGUIContext* hostContext):
_hostContext(hostContext) {}

void 
FBUndoStateContainer::Undo()
{
  FB_ASSERT(CanUndo());
  SwapContextForItem(_hostContext, _items[_position - 1]);
  _position--;
}

void 
FBUndoStateContainer::Redo()
{
  FB_ASSERT(CanRedo());
  SwapContextForItem(_hostContext, _items[_position]);
  _position++;
}

void 
FBUndoStateContainer::Snapshot(std::string const& action)
{
  _items.erase(_items.begin() + _position, _items.end());
  FBUndoItem item(*_hostContext->Topo());
  item.action = action;
  item.state.CopyFrom(_hostContext, false);
  item.patchName = _hostContext->PatchName();
  _items.push_back(std::move(item));
  while (_items.size() > _hostContext->Topo()->static_->maxUndoSize)
    _items.erase(_items.begin());
  _position = (int)_items.size();
}