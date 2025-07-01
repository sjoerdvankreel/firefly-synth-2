#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

FBUndoStateContainer::
FBUndoStateContainer(FBHostGUIContext* hostContext):
_hostContext(hostContext) {}

void 
FBUndoStateContainer::Undo()
{
  FB_ASSERT(CanUndo());
  FBUndoItem item(*_hostContext->Topo());
  item.action = _undoStack.top().action;
  item.state.CopyFrom(_hostContext);
  _redoStack.push(std::move(item));
  _undoStack.top().state.CopyTo(_hostContext);
  _undoStack.pop();
}

void 
FBUndoStateContainer::Redo()
{
  FB_ASSERT(CanRedo());
  FBUndoItem item(*_hostContext->Topo());
  item.action = _redoStack.top().action;
  item.state.CopyFrom(_hostContext);
  _undoStack.push(std::move(item));
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
  _undoStack.push(std::move(item));
}