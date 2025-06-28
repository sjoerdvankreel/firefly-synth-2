#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <vector>

class FBHostGUIContext;

class FBUndoStateContainer final
{
  int _pos = 0;
  int _activeActionCount = 0;
  std::vector<std::string> _actions = {};
  std::vector<FBScalarStateContainer> _state = {};

  FBHostGUIContext* const _hostContext;
  int const _maxUndoSize;

public:
  void Undo();
  void Redo();
  void Clear();
  void EndAction();
  void BeginAction(std::string const& name);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBUndoStateContainer);
  FBUndoStateContainer(FBHostGUIContext* hostContext);

  bool CanUndo() const { return _state.size() > 0 && _pos != 0; };
  bool CanRedo() const { return _state.size() > 0 && _pos != _state.size(); }
};