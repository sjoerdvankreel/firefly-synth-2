#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGUI.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component*
MakeModMatrixSectionAll(FBPlugGUI* plugGUI, FFModuleType moduleType)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> rowSizes(FFModMatrixSlotCount + 1, 1);
  std::vector<int> columnSizes((int)FFModMatrixParam::Count, 1);
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, rowSizes, columnSizes);
  auto opType0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, opType0));
  auto source0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, 0 } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, source0));
  auto target0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, target0));
  auto bipolar0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Bipolar, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bipolar0));
  auto amount0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Amount, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amount0));

  for (int i = 0; i < FFModMatrixSlotCount; i++)
  {
    auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, i } });
    grid->Add(1 + i, 0, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));
    auto source = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, i } });
    grid->Add(1 + i, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, source));
    auto target = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, i } });
    grid->Add(1 + i, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target));
    auto bipolar = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Bipolar, i } });
    grid->Add(1 + i, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bipolar));
    auto amount = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Amount, i } });
    grid->Add(1 + i, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { FFModMatrixSlotCount + 1, (int)FFModMatrixParam::Count } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeModMatrixGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::MatrixSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  tabs->AddModuleTab(false, { (int)FFModuleType::VMatrix, 0 }, MakeModMatrixSectionAll(plugGUI, FFModuleType::VMatrix));
  tabs->AddModuleTab(false, { (int)FFModuleType::GMatrix, 0 }, MakeModMatrixSectionAll(plugGUI, FFModuleType::GMatrix));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}