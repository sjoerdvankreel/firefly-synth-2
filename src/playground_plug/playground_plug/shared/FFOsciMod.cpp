#include <playground_plug/shared/FFOsciMod.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMGUI.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMGUI.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static std::map<std::pair<int, int>, int>
MakeSourceAndTargetToSlot();
static std::array<std::pair<int, int>, FFOsciModSlotCount>
MakeSlotToSourceAndTarget();

static std::map<std::pair<int, int>, int>
sourceAndTargetToSlot = MakeSourceAndTargetToSlot();
static std::array<std::pair<int, int>, FFOsciModSlotCount> 
slotToSourceAndTarget = MakeSlotToSourceAndTarget();

std::map<std::pair<int, int>, int> const&
FFOsciModSourceAndTargetToSlot()
{
  return sourceAndTargetToSlot;
}

std::array<std::pair<int, int>, FFOsciModSlotCount> const&
FFOsciModSlotToSourceAndTarget()
{
  return slotToSourceAndTarget;
}

static std::map<std::pair<int, int>, int>
MakeSourceAndTargetToSlot()
{
  int slot = 0;
  std::map<std::pair<int, int>, int> result = {};
  for (int tgt = 0; tgt < FFOsciCount; tgt++)
    for (int src = 0; src <= tgt; src++)
      result[{ src, tgt }] = slot++;
  return result;
}

static std::array<std::pair<int, int>, FFOsciModSlotCount>
MakeSlotToSourceAndTarget()
{
  int slot = 0;
  std::array<std::pair<int, int>, FFOsciModSlotCount> result;
  for (int tgt = 0; tgt < FFOsciCount; tgt++)
    for (int src = 0; src <= tgt; src++)
      result[slot++] = { src, tgt };
  return result;
}

std::vector<int>
FFMakeOsciModUIColumnSizes()
{
  std::vector<int> result = {};
  result.push_back(0);
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    result.push_back(0);
    result.push_back(1);
  }
  return result;
}

void
FFMarkOsciModUIGridSections(FBGridComponent* grid)
{
  int x = 0;
  int w = 2;
  grid->MarkSection({ 0, 0, 2, 1 });
  for (int i = 0; i < FFOsciCount; i++)
  {
    grid->MarkSection({ 0, 1 + x, 2, w });
    x += 2 * (i + 1);
    w += 2;
  }
}

std::string
FFOsciModMakeSourceAndTargetText(int slot)
{
  auto src = std::to_string(FFOsciModSlotToSourceAndTarget()[slot].first + 1);
  auto tgt = std::to_string(FFOsciModSlotToSourceAndTarget()[slot].second + 1);
  return src + ">" + tgt;
};

Component*
FFMakeOsciModGUI(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  TabbedComponent* result = plugGUI->StoreComponent<TabbedComponent>(TabbedButtonBar::Orientation::TabsAtTop);
  auto const& amName = topo->static_.modules[(int)FFModuleType::OsciAM].name;
  result->addTab(amName, Colours::black, FFMakeOsciAMGUI(plugGUI), false);
  auto const& fmName = topo->static_.modules[(int)FFModuleType::OsciFM].name;
  result->addTab(fmName, Colours::black, FFMakeOsciFMGUI(plugGUI), false);
  return result;
}