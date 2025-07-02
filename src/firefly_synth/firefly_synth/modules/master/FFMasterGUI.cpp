#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/master/FFMasterGUI.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>

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
MakeMasterSectionAll(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 1 } );
  auto hostSmoothTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::HostSmoothTime, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hostSmoothTime));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hostSmoothTime, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeMasterGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_.modules[(int)FFModuleType::Master].name;
  tabs->addTab(name, {}, MakeMasterSectionAll(plugGUI), false);
  return tabs;
}