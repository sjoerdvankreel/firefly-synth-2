#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <stack>
#include <deque>

class FBHostGUIContext;

struct FBUndoItem final
{
  std::string action = {};
  FBScalarStateContainer state;

  FB_NOCOPY_MOVE_NODEFCTOR(FBUndoItem);
  FBUndoItem(FBRuntimeTopo const& topo): state(topo) {}
};

class FBUndoStateContainer final
{
  std::deque<FBUndoItem> _undoDeque = {};
  std::stack<FBUndoItem> _redoStack = {};
  FBHostGUIContext* const _hostContext;

public:
  void Undo();
  void Redo();
  void Snapshot(std::string const& action);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBUndoStateContainer);
  FBUndoStateContainer(FBHostGUIContext* hostContext);

  bool CanRedo() const { return _redoStack.size() > 0; }
  bool CanUndo() const { return _undoDeque.size() > 0; }
  std::string const& RedoAction() { FB_ASSERT(CanRedo()); return _redoStack.top().action; }
  std::string const& UndoAction() { FB_ASSERT(CanUndo()); return _undoDeque.front().action; }
};