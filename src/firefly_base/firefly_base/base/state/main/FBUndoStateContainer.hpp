#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <vector>

class FBHostGUIContext;

class FBUndoStateContainer final
{
  int _position = -1;
  int _activeActionCount = 0;
  bool _isActive = false;
  std::vector<std::string> _actions = {};
  std::vector<FBScalarStateContainer> _state = {};
  FBHostGUIContext* const _hostContext;

public:
  void Undo();
  void Redo();
  void Clear();
  
  void DeactivateNow();
  void ActivateAsync();

  void EndAction();
  void BeginAction(std::string const& name);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBUndoStateContainer);
  FBUndoStateContainer(FBHostGUIContext* hostContext);

  std::string const& RedoAction() { FB_ASSERT(CanRedo()); return _actions[_position]; }
  std::string const& UndoAction() { FB_ASSERT(CanUndo()); return _actions[_position - 1]; }
  bool CanRedo() const { return _state.size() > 0 && _position >= 0 && _position < _state.size(); }
  bool CanUndo() const { return _state.size() > 0 && _position > 0 && _position <= _state.size(); };
};