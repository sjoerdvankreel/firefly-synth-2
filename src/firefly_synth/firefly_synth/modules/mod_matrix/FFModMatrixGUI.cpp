#include <firefly_synth/gui/FFPlugGUI.hpp>
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
MakeModMatrixGUI(bool global, int offset, FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> rowSizes(FFModMatrixGlobalSlotCount + 1, 1);
  std::vector<int> columnSizes = { 0, 1, 1, 2, 0 };
  auto moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, rowSizes, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(global? "Global": "Voice"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>("Source"));
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>("Scale"));
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>("Target"));
  grid->Add(0, 4, plugGUI->StoreComponent<FBAutoSizeLabel>("Amt"));
  for (int i = 0; i < FFModMatrixGlobalSlotCount; i++)
  {
    auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, offset + i } });
    grid->Add(1 + i, 0, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));
    
    std::function<void(int)> sourceOrScaleChanged = [plugGUI, global](int itemResultId) {
      if (itemResultId != 0)
      {
        auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*plugGUI->HostContext()->Topo()->static_);
        auto const& sources = global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
        auto const& moduleIndices = sources[itemResultId - 1].indices.module;
        if(moduleIndices.index != -1)
          plugGUI->SwitchGraphToModule(moduleIndices.index, moduleIndices.slot);
      }
    };

    auto source = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, offset + i } });
    auto* sourceCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, source);
    sourceCombo->valueChangedByUserAction = sourceOrScaleChanged;
    grid->Add(1 + i, 1, sourceCombo);
    
    auto scale = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Scale, offset + i } });
    auto* scaleCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, scale);
    scaleCombo->valueChangedByUserAction = sourceOrScaleChanged;
    grid->Add(1 + i, 2, scaleCombo);

    auto target = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, offset + i } });
    auto* targetCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target);
    targetCombo->valueChangedByUserAction = [plugGUI, global](int itemResultId) {
      if (itemResultId != 0)
      {
        auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*plugGUI->HostContext()->Topo()->static_);
        auto const& targets = global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;
        auto const& moduleIndices = targets[itemResultId - 1].module;
        if (moduleIndices.index != -1)
          plugGUI->SwitchGraphToModule(moduleIndices.index, moduleIndices.slot);
      }
    };
      
    grid->Add(1 + i, 3, targetCombo);

    auto amount = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Amount, offset + i } });
    grid->Add(1 + i, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { FFModMatrixGlobalSlotCount + 1, (int)FFModMatrixParam::Count } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeModMatrixGUI(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(false, std::vector<int> { { 1 } }, std::vector<int> { { 1, 1, 1 } });
  grid->Add(0, 0, MakeModMatrixGUI(false, 0, plugGUI));
  grid->Add(0, 1, MakeModMatrixGUI(false, FFModMatrixGlobalSlotCount, plugGUI));
  grid->Add(0, 2, MakeModMatrixGUI(true, 0, plugGUI));
  return grid;
}