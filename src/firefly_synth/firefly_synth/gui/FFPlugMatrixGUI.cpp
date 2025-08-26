#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/gui/FFPlugMatrixGUI.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

Component*
FFMakePlugMatrixGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 1 });
  auto toggle = plugGUI->StoreComponent<FBAutoSizeButton>("Show Matrix");
  toggle->onClick = [plugGUI, toggle]() { 
    if (plugGUI->ToggleMatrix())
      toggle->setButtonText("Hide Matrix");
    else
      toggle->setButtonText("Show Matrix"); };
  grid->Add(0, 0, toggle);
  grid->MarkSection({ { 0, 0 }, { 1, 1 } });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
}