#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

FBUndoStateContainer::
FBUndoStateContainer(FBHostGUIContext* hostContext):
_hostContext(hostContext) {}

void 
FBUndoStateContainer::Undo()
{
  FB_ASSERT(CanUndo());
  FBScalarStateContainer temp(*_hostContext->Topo());
  temp.CopyFrom(_hostContext);
  _items[_position - 1].state.CopyTo(_hostContext);
  _items[_position - 1].state.CopyFrom(temp);
  _position--;
}

void 
FBUndoStateContainer::Redo()
{
  FB_ASSERT(CanRedo());
  FBScalarStateContainer temp(*_hostContext->Topo());
  temp.CopyFrom(_hostContext);
  _items[_position].state.CopyTo(_hostContext);
  _items[_position].state.CopyFrom(temp);
  _position++;
}

void 
FBUndoStateContainer::Snapshot(std::string const& action)
{
  _items.erase(_items.begin() + _position, _items.end());
  FBUndoItem item(*_hostContext->Topo());
  item.action = action;
  item.state.CopyFrom(_hostContext);
  _items.push_back(std::move(item));
  while (_items.size() > _hostContext->Topo()->static_->maxUndoSize)
    _items.erase(_items.begin());
  _position = (int)_items.size();
}