#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/master/FFMasterGUI.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeMasterSectionMain(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 1, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto bend = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBend, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bend));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bend, Slider::SliderStyle::LinearHorizontal));
  auto bendRange = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBendRange, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bendRange));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bendRange));
  auto mod = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::ModWheel, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mod));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mod, Slider::SliderStyle::LinearHorizontal));
  auto bendTarget = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBendTarget, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bendTarget));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bendTarget));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeMasterSectionAux(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = {};
  for (int i = 0; i < FFMasterAuxCount / 2; i++)
  {
    columnSizes.push_back(0);
    columnSizes.push_back(0);
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  for (int i = 0; i < FFMasterAuxCount; i++)
  {
    int row = i / (FFMasterAuxCount / 2);
    int col = i % (FFMasterAuxCount / 2);
    auto aux = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::Aux, i } });
    grid->Add(row, col * 2 + 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, aux));
    grid->Add(row, col * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, aux, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 2 * FFMasterAuxCount / 2 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeMasterTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 1, 0 });
  grid->Add(0, 0, MakeMasterSectionMain(plugGUI));
  grid->Add(0, 1, MakeMasterSectionAux(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeMasterGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::Master].name;
  tabs->addTab(name, {}, MakeMasterTab(plugGUI), false);
  return tabs;
}