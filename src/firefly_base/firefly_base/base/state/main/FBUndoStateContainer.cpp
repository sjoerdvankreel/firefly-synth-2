#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

FBUndoStateContainer::
FBUndoStateContainer(FBHostGUIContext* hostContext):
_hostContext(hostContext) {}

void
FBUndoStateContainer::Clear()
{
  FB_ASSERT(_activeActionCount == 0);
  _pos = 0;
  _state.clear();
  _actions.clear();
}

void 
FBUndoStateContainer::Undo()
{
  FB_ASSERT(CanUndo());
  FB_ASSERT(_activeActionCount == 0);
  FB_ASSERT(0 < _pos && _pos <= _state.size());
  _state[_pos - 1].CopyTo(_hostContext);
  _pos--;
}

void 
FBUndoStateContainer::Redo()
{
  FB_ASSERT(CanRedo());
  FB_ASSERT(_activeActionCount == 0);
  FB_ASSERT(0 <= _pos && _pos < _state.size());
  _state[_pos].CopyTo(_hostContext);
  _pos++;
}

void 
FBUndoStateContainer::BeginAction(std::string const& action)
{
  if (_activeActionCount == 0)
  {
    _state.erase(_state.begin() + _pos, _state.end());
    _actions.erase(_actions.begin() + _pos, _actions.end());
    _actions.push_back(action);
  }
  _activeActionCount++;
}

void
FBUndoStateContainer::EndAction()
{
  FB_ASSERT(_activeActionCount > 0);
  FB_ASSERT(_pos == _state.size());
  _activeActionCount--;
  if (_activeActionCount != 0)
    return;
  _pos++;
  auto& state = _state.emplace_back(*_hostContext->Topo());
  state.CopyFrom(_hostContext);
  if (_state.size() > _hostContext->Topo()->static_.maxUndoSize)
  {
    _state.erase(_state.begin());
    _actions.erase(_actions.begin());
    _pos--;
  }
}