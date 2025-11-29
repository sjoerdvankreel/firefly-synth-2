#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/panic/FFPanicGUI.hpp>
#include <firefly_synth/modules/panic/FFPanicTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

Component*
FFMakePanicGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0 } );
  auto panicButton = plugGUI->StoreComponent<FBAutoSizeButton>("Panic");
  panicButton->setTooltip("Reset Voices And Delay Lines");
  panicButton->onClick = [plugGUI]() { dynamic_cast<FFPlugGUI&>(*plugGUI).FlushAudio(); };
  grid->Add(0, 0, panicButton);
  grid->MarkSection({ { 0, 0 }, { 1, 1 } });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
}