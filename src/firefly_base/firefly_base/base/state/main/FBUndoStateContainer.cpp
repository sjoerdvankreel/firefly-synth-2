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
  _pos = 0;
  _state.clear();
  _actions.clear();
  _activeActionCount = 0;
}

void 
FBUndoStateContainer::Undo()
{
  if (!_isActive) return;
  FB_ASSERT(CanUndo());
  FB_ASSERT(_activeActionCount == 0);
  FB_ASSERT(0 < _pos && _pos <= _state.size());
  _isActive = false;
  _state[_pos - 1].CopyTo(_hostContext);
  _pos--;
  ActivateAsync();
}

void 
FBUndoStateContainer::Redo()
{
  if (!_isActive) return;
  FB_ASSERT(CanRedo());
  FB_ASSERT(_activeActionCount == 0);
  FB_ASSERT(0 <= _pos && _pos < _state.size());
  _isActive = false;
  _state[_pos].CopyTo(_hostContext);
  _pos++;
  ActivateAsync();
}

void 
FBUndoStateContainer::BeginAction(std::string const& action)
{
  if (!_isActive) return;
  if (_activeActionCount == 0)
  {
    _state.erase(_state.begin() + _pos, _state.end());
    _actions.erase(_actions.begin() + _pos, _actions.end());
    _actions.push_back(action);
    auto& state = _state.emplace_back(*_hostContext->Topo());
    state.CopyFrom(_hostContext);
    _pos++;
  }
  _activeActionCount++;
}

void
FBUndoStateContainer::EndAction()
{
  if (!_isActive) return;
  FB_ASSERT(_activeActionCount > 0);
  FB_ASSERT(_pos == _state.size());
  _activeActionCount--;
  if (_activeActionCount != 0)
    return;
  if (_state.size() > _hostContext->Topo()->static_.maxUndoSize)
  {
    _state.erase(_state.begin());
    _actions.erase(_actions.begin());
    _pos--;
  }
}