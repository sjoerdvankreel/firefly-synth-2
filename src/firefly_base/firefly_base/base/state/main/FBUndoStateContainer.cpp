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
  FBUndoItem item(*_hostContext->Topo());
  item.action = _undoDeque.front().action;
  item.state.CopyFrom(_hostContext);
  _redoStack.push(std::move(item));
  _undoDeque.front().state.CopyTo(_hostContext);
  _undoDeque.pop_front();
}

void 
FBUndoStateContainer::Redo()
{
  FB_ASSERT(CanRedo());
  FBUndoItem item(*_hostContext->Topo());
  item.action = _redoStack.top().action;
  item.state.CopyFrom(_hostContext);
  _undoDeque.push_front(std::move(item));
  _redoStack.top().state.CopyTo(_hostContext);
  _redoStack.pop();
}

void 
FBUndoStateContainer::Snapshot(std::string const& action)
{
  _redoStack = {};
  FBUndoItem item(*_hostContext->Topo());
  item.action = action;
  item.state.CopyFrom(_hostContext);
  _undoDeque.push_front(std::move(item));
  while (_undoDeque.size() > _hostContext->Topo()->static_->maxUndoSize)
    _undoDeque.pop_back();
}