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
MakeModMatrixGUI(bool global, int offset, FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> rowSizes(FFModMatrixGlobalSlotCount + 1, 1);
  std::vector<int> columnSizes = { 0, 0, 1, 0 };
  auto moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, rowSizes, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Op"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>("Source"));
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(global? "Global Target": "Voice Target"));
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>("Amt"));
  for (int i = 0; i < FFModMatrixGlobalSlotCount; i++)
  {
    auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, offset + i } });
    grid->Add(1 + i, 0, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));
    auto source = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, offset + i } });
    grid->Add(1 + i, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, source));
    auto target = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, offset + i } });
    grid->Add(1 + i, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target));
    auto amount = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Amount, offset + i } });
    grid->Add(1 + i, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { FFModMatrixGlobalSlotCount + 1, (int)FFModMatrixParam::Count } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeModMatrixGUI(FBPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(false, std::vector<int> { { 1 } }, std::vector<int> { { 1, 1, 1 } });
  grid->Add(0, 0, MakeModMatrixGUI(false, 0, plugGUI));
  grid->Add(0, 1, MakeModMatrixGUI(false, FFModMatrixGlobalSlotCount, plugGUI));
  grid->Add(0, 2, MakeModMatrixGUI(true, 0, plugGUI));
  return grid;
}