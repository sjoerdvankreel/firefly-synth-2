#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <stack>
#include <memory>

using namespace juce;

struct MenuBuilder
{
  bool checked = false;
  bool enabled = false;
  std::string name = {};
  std::unique_ptr<PopupMenu> menu = {};
};

FBHostGUIContext::
FBHostGUIContext():
_undoState(this) {}

void
FBHostGUIContext::BeginAudioParamChange(int index)
{
  DoBeginAudioParamChange(index);
}

void
FBHostGUIContext::EndAudioParamChange(int index)
{
  DoEndAudioParamChange(index);
}

void
FBHostGUIContext::PerformAudioParamEdit(int index, double normalized)
{
  std::string name = Topo()->audio.params[index].longName;
  if(!Topo()->audio.params[index].static_.output)
    UndoState().BeginAction("Change " + name);
  DoPerformAudioParamEdit(index, normalized);
  if (!Topo()->audio.params[index].static_.output)
    UndoState().EndAction();
}

void
FBHostGUIContext::PerformImmediateAudioParamEdit(int index, double normalized)
{
  BeginAudioParamChange(index);
  PerformAudioParamEdit(index, normalized);
  EndAudioParamChange(index);
}

void 
FBHostGUIContext::ClearModuleAudioParams(FBTopoIndices const& moduleIndices)
{
  std::string name = Topo()->ModuleAtTopo(moduleIndices)->name;
  UndoState().BeginAction("Clear " + name);
  auto const& staticModule = Topo()->static_.modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
  {
    auto const& staticParam = staticModule.params[p];
    for (int s = 0; s < staticParam.slotCount; s++)
    {
      auto runtimeParam = Topo()->audio.ParamAtTopo({ moduleIndices, { p, s } });
      double normalized = staticParam.DefaultNormalizedByText(moduleIndices.slot, s);
      PerformImmediateAudioParamEdit(runtimeParam->runtimeParamIndex, normalized);
    }
  }
  UndoState().EndAction();
}

void 
FBHostGUIContext::CopyModuleAudioParams(FBTopoIndices const& moduleIndices, int toSlot)
{
  std::string name = Topo()->ModuleAtTopo(moduleIndices)->name;
  UndoState().BeginAction("Copy " + name);
  auto const& staticModule = Topo()->static_.modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
  {
    auto const& staticParam = staticModule.params[p];
    for (int s = 0; s < staticParam.slotCount; s++)
    {
      auto fromRuntimeParam = Topo()->audio.ParamAtTopo({ moduleIndices, { p, s } });
      auto toRuntimeParam = Topo()->audio.ParamAtTopo({ { moduleIndices.index, toSlot }, { p, s } });
      auto fromValue = GetAudioParamNormalized(fromRuntimeParam->runtimeParamIndex);
      PerformImmediateAudioParamEdit(toRuntimeParam->runtimeParamIndex, fromValue);
    }
  }
  UndoState().EndAction();
}

std::unique_ptr<PopupMenu>
FBMakeHostContextMenu(std::vector<FBHostContextMenuItem> const& items)
{
  std::stack<MenuBuilder> builders = {};
  builders.emplace();
  builders.top().menu = std::make_unique<PopupMenu>();
  for (int i = 0; i < items.size(); i++)
    if (items[i].subMenuStart)
    {
      builders.emplace();
      builders.top().name = items[i].name;
      builders.top().checked = items[i].checked;
      builders.top().enabled = items[i].enabled;
      builders.top().menu = std::make_unique<PopupMenu>();
    } else if (items[i].subMenuEnd)
    {
      auto builder = std::move(builders.top());
      builders.pop();
      builders.top().menu->addSubMenu(builder.name, *builder.menu, builder.enabled, nullptr, builder.checked);
    } else if (items[i].separator)
      builders.top().menu->addSeparator();
    else
      builders.top().menu->addItem(i + 1, items[i].name, items[i].enabled, items[i].checked);
  FB_ASSERT(builders.size() == 1);
  return std::move(builders.top().menu);
}