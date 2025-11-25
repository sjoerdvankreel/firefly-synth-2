#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <vector>

class FBHostGUIContext;

struct FBUndoItem final
{
  std::string action = {};
  bool isBeforePatchChange = {};
  FBScalarStateContainer state;

  FB_NOCOPY_MOVE_NODEFCTOR(FBUndoItem);
  FBUndoItem(FBRuntimeTopo const& topo): state(topo) {}
};

class FBUndoStateContainer final
{
  int _position = 0;
  std::vector<FBUndoItem> _items = {};
  FBHostGUIContext* const _hostContext;

public:
  void Undo();
  void Redo();
  bool RevertToLastPatchLoad();
  void Snapshot(bool isBeforePatchChange, std::string const& action);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBUndoStateContainer);
  FBUndoStateContainer(FBHostGUIContext* hostContext);

  bool CanUndo() const { return _items.size() > 0 && _position > 0; }
  bool CanRedo() const { return _items.size() > 0 && _position < (int)_items.size(); }
  std::string const& RedoAction() const { FB_ASSERT(CanRedo()); return _items[_position].action; }
  std::string const& UndoAction() const { FB_ASSERT(CanUndo()); return _items[_position - 1].action; }
};