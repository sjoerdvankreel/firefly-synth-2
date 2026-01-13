#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/other/FFOtherGUI.hpp>
#include <firefly_synth/modules/other/FFOtherTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/controls/FBInstanceNameEditor.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

Component*
FFMakeOtherGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0 } );
  grid->Add(0, 0, plugGUI->StoreComponent<FBFillerComponent>(1, 1));
  auto instanceNameEditor = plugGUI->StoreComponent<FBInstanceNameEditor>(plugGUI, 100);
  plugGUI->onInstanceNameChanged = [instanceNameEditor](auto const& name) { instanceNameEditor->setText(name, false); };
  grid->Add(0, 1, instanceNameEditor);
  auto panicButton = plugGUI->StoreComponent<FBAutoSizeButton>(plugGUI, "Panic");
  panicButton->setTooltip("Reset Voices And Delay Lines"); 
  panicButton->onClick = [plugGUI]() { dynamic_cast<FFPlugGUI&>(*plugGUI).FlushAudio(); };
  grid->Add(0, 2, panicButton);
  grid->Add(0, 3, plugGUI->StoreComponent<FBFillerComponent>(1, 1));
  grid->MarkSection({ { 0, 0 }, { 1, 4 } }, FBGridSectionMark::BackgroundAndBorder);
  auto section = plugGUI->StoreComponent<FBSectionComponent>(true, grid);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::Other, 0, section);
}