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

Component*
FFMakeModMatrixGUI(bool global, FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
  std::vector<int> rowSizes(FFModMatrixSlotCount / 2 + 1, 1);
  std::vector<int> columnSizes = { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 };
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, rowSizes, columnSizes);
  for (int i = 0; i < 2; i++)
  {
    auto opType0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, 0 } });
    grid->Add(0, 5 * i + 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, opType0));
    auto source0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, 0 } });
    grid->Add(0, 5 * i + 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, source0));
    auto target0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, 0 } });
    grid->Add(0, 5 * i + 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, target0));
    auto bipolar0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Bipolar, 0 } });
    grid->Add(0, 5 * i + 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bipolar0));
    auto amount0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Amount, 0 } });
    grid->Add(0, 5 * i + 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amount0));
  }
  for (int i = 0; i < FFModMatrixSlotCount; i++)
  {
    int col = 0;
    int row = i;
    if (i >= FFModMatrixSlotCount / 2)
    {
      col = 5;
      row -= FFModMatrixSlotCount / 2;
    }
    auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, i } });
    grid->Add(1 + row, col + 0, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));
    auto source = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, i } });
    grid->Add(1 + row, col + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, source));
    auto target = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, i } });
    grid->Add(1 + row, col + 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target));
    auto bipolar = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Bipolar, i } });
    grid->Add(1 + row, col + 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, bipolar));
    auto amount = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Amount, i } });
    grid->Add(1 + row, col + 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { FFModMatrixSlotCount / 2 + 1, (int)FFModMatrixParam::Count * 2 } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}