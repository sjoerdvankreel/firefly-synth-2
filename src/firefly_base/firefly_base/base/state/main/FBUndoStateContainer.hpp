#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <vector>

class FBHostGUIContext;

class FBUndoStateContainer final
{
  int _pos = 0;
  int _activeActionCount = 0;
  bool _discard = false;
  bool _active = false;
  std::vector<std::string> _actions = {};
  std::vector<FBScalarStateContainer> _state = {};
  FBHostGUIContext* const _hostContext;

public:
  void Undo();
  void Redo();
  void Clear();
  void EndAction();
  void EndDiscard();
  void BeginDiscard();
  void BeginAction(std::string const& name);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBUndoStateContainer);
  FBUndoStateContainer(FBHostGUIContext* hostContext);

  void Activate();
  bool CanUndo() const { return _active && _state.size() > 0 && _pos != 0; };
  bool CanRedo() const { return _active && _state.size() > 0 && _pos != _state.size(); }
  std::string const& RedoAction() { FB_ASSERT(CanRedo()); return _actions[_pos]; }
  std::string const& UndoAction() { FB_ASSERT(CanUndo()); return _actions[_pos - 1]; }
};