#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

FBUndoStateContainer::
FBUndoStateContainer(FBHostGUIContext* hostContext):
_hostContext(hostContext) {}

void
FBUndoStateContainer::DeactivateNow()
{
  Clear();
  _isActive = false;
}

void
FBUndoStateContainer::ActivateAsync()
{
  juce::MessageManager::callAsync(
    [this]() { _isActive = true; });
}

void
FBUndoStateContainer::Clear()
{
  if (!_isActive) return;
  _undoStack = {};
  _redoStack = {};
  _activeActionCount = 0;
}

void 
FBUndoStateContainer::Undo()
{
  if (!_isActive) return;
  FB_ASSERT(CanUndo());
  FB_ASSERT(_activeActionCount == 0);
  _isActive = false;
  FBUndoItem item(*_hostContext->Topo());
  item.action = _undoStack.top().action;
  item.state.CopyFrom(_hostContext);
  _redoStack.push(std::move(item));
  _undoStack.top().state.CopyTo(_hostContext);
  _undoStack.pop();
  ActivateAsync();
}

void 
FBUndoStateContainer::Redo()
{
  if (!_isActive) return;
  FB_ASSERT(CanRedo());
  FB_ASSERT(_activeActionCount == 0);
  _isActive = false;
  FBUndoItem item(*_hostContext->Topo());
  item.action = _redoStack.top().action;
  item.state.CopyFrom(_hostContext);
  _undoStack.push(std::move(item));
  _redoStack.top().state.CopyTo(_hostContext);
  _redoStack.pop();
  ActivateAsync();
}

void 
FBUndoStateContainer::BeginAction(std::string const& action)
{
  if (!_isActive) return;
  if (_activeActionCount == 0)
  {
    _redoStack = {};
    FBUndoItem item(*_hostContext->Topo());
    item.action = action;
    item.state.CopyFrom(_hostContext);
    _undoStack.push(std::move(item));
  }
  _activeActionCount++;
}

void
FBUndoStateContainer::EndAction()
{
  if (!_isActive) return;
  FB_ASSERT(_activeActionCount > 0);
  _activeActionCount--;
  if (_activeActionCount != 0)
    return;
  // todo maxsize
}