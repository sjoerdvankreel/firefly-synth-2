#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/output/FFOutputGUI.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBOutputLabel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeOutputSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0 } );
  auto voices = topo->audio.ParamAtTopo({ (int)FFModuleType::Output, moduleSlot, (int)FFOutputParam::Voices, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voices));
  grid->Add(0, 1, plugGUI->StoreComponent<FBOutputParamLabel>(plugGUI, voices, "0", std::to_string(FBMaxVoices)));
  grid->MarkSection({ 0, 0, 1, 2 });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
} 

Component*
FFMakeOutputGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Output, MakeOutputSectionAll);
}