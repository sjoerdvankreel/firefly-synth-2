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
  _position = -1;
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
  FB_ASSERT(0 < _position && _position <= _state.size());
  _isActive = false;
  _state[_position - 1].CopyTo(_hostContext);
  _position--;
  ActivateAsync();
}

void 
FBUndoStateContainer::Redo()
{
  if (!_isActive) return;
  FB_ASSERT(CanRedo());
  FB_ASSERT(_activeActionCount == 0);
  FB_ASSERT(0 <= _position && _position < _state.size());
  _isActive = false;
  _state[_position + 1].CopyTo(_hostContext);
  _position++;
  ActivateAsync();
}

void 
FBUndoStateContainer::BeginAction(std::string const& action)
{
  if (!_isActive) return;
  if (_activeActionCount == 0)
  {
    if (_state.size() != 0)
    {
      _state.erase(_state.begin() + _position, _state.end());
      _actions.erase(_actions.begin() + _position, _actions.end());
    }
    _actions.push_back(action);
    auto& state = _state.emplace_back(*_hostContext->Topo());
    state.CopyFrom(_hostContext);
    _position = static_cast<int>(_state.size());
  }
  _activeActionCount++;
}

void
FBUndoStateContainer::EndAction()
{
  if (!_isActive) return;
  FB_ASSERT(_activeActionCount > 0);
  FB_ASSERT(_position == _state.size());
  _activeActionCount--;
  if (_activeActionCount != 0)
    return;
  if (_state.size() > _hostContext->Topo()->static_.maxUndoSize)
  {
    // TODO
    _state.erase(_state.begin());
    _actions.erase(_actions.begin());
    _position--;
  }
}