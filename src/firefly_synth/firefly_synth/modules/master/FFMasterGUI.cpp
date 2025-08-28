#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/master/FFMasterGUI.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
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
  std::vector<int> columnSizes = { 0, 1 };
  for (int i = 0; i < FFMasterAuxCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto tuning = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::TuningMode, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuning));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tuning));
  auto smooth = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::HostSmoothTime, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smooth));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smooth, Slider::SliderStyle::LinearHorizontal));
  for (int i = 0; i < FFMasterAuxCount ; i++)
  {
    int row = i / (FFMasterAuxCount / 2);
    int col = i % (FFMasterAuxCount / 2);
    auto aux = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::Aux, i } });
    grid->Add(row, 2 + col * 2 + 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, aux));
    grid->Add(row, 2 + col * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, aux, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 2 + 2 * FFMasterAuxCount / 2 } });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
}

Component*
FFMakeMasterGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::Master].name;
  tabs->addTab(name, {}, MakeMasterSectionAll(plugGUI), false);
  return tabs;
}