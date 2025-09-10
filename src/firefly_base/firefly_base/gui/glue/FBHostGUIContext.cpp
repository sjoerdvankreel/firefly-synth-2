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
  DoPerformAudioParamEdit(index, normalized);
}

void
FBHostGUIContext::PerformImmediateAudioParamEdit(FBParamTopoIndices const& indices, double normalized)
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  PerformImmediateAudioParamEdit(param->runtimeParamIndex, normalized);
}

void
FBHostGUIContext::PerformImmediateAudioParamEdit(int index, double normalized)
{
  BeginAudioParamChange(index);
  PerformAudioParamEdit(index, normalized);
  EndAudioParamChange(index);
}

double
FBHostGUIContext::GetAudioParamNormalized(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  return GetAudioParamNormalized(param->runtimeParamIndex);
}

bool 
FBHostGUIContext::GetAudioParamBool(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
}

bool
FBHostGUIContext::GetGUIParamBool(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
}

int 
FBHostGUIContext::GetGUIParamDiscrete(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Discrete().NormalizedToPlainFast(static_cast<float>(normalized));
}

void 
FBHostGUIContext::DefaultAudioParam(FBParamTopoIndices const& indices)
{
  auto runtimeParam = Topo()->audio.ParamAtTopo(indices);
  double normalized = runtimeParam->DefaultNormalizedByText();
  PerformImmediateAudioParamEdit(runtimeParam->runtimeParamIndex, normalized);
}

void 
FBHostGUIContext::CopyAudioParam(FBParamTopoIndices const& from, FBParamTopoIndices const& to)
{
  auto fromRuntimeParam = Topo()->audio.ParamAtTopo(from);
  auto toRuntimeParam = Topo()->audio.ParamAtTopo(to);
  auto fromValue = GetAudioParamNormalized(fromRuntimeParam->runtimeParamIndex);
  PerformImmediateAudioParamEdit(toRuntimeParam->runtimeParamIndex, fromValue);
}

void 
FBHostGUIContext::ClearModuleAudioParams(FBTopoIndices const& moduleIndices)
{  
  auto const& staticModule = Topo()->static_->modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
    for (int s = 0; s < staticModule.params[p].slotCount; s++)
      DefaultAudioParam({ moduleIndices, { p, s } });
}

void 
FBHostGUIContext::CopyModuleAudioParams(FBTopoIndices const& moduleIndices, int toSlot)
{  
  auto const& staticModule = Topo()->static_->modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
    for (int s = 0; s < staticModule.params[p].slotCount; s++)
      CopyAudioParam({ moduleIndices, { p, s } }, { { moduleIndices.index, toSlot }, { p, s } });
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