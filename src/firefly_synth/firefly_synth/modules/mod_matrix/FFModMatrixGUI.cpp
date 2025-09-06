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
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component*
MakeModMatrixTopGUI(bool global, FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> rowSizes = { { 1 } };
  std::vector<int> columnSizes = { 0, 0, 0, 0, 1, 0, 0 };
  auto moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, 6, rowSizes, columnSizes);
  auto slots = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Slots, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(global ? "Global" : "Voice"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, slots));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, slots, Slider::SliderStyle::RotaryVerticalDrag));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamDisplayLabel>(plugGUI, slots, std::to_string(global ? FFModMatrixGlobalMaxSlotCount : FFModMatrixVoiceMaxSlotCount)));
  
  auto clean = plugGUI->StoreComponent<FBAutoSizeButton>("Clean");
  grid->Add(0, 5, clean);
  clean->onClick = [plugGUI, global]() {
    std::vector<double> amtNorm = {};
    std::vector<double> scaleNorm = {};
    std::vector<double> targetNorm = {};
    std::vector<double> opTypeNorm = {};
    std::vector<double> sourceNorm = {};
    auto* hostContext = plugGUI->HostContext();
    int moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
    int maxSlotCount = global ? FFModMatrixGlobalMaxSlotCount : FFModMatrixVoiceMaxSlotCount;
    FBTopoIndices modIndices = { moduleType, 0 };
    std::string name = hostContext->Topo()->ModuleAtTopo(modIndices)->name;
    hostContext->UndoState().Snapshot("Clean " + name);
    for (int i = 0; i < maxSlotCount; i++)
    {
      bool opTypeOn = hostContext->GetAudioParamList<int>({ modIndices, { (int)FFModMatrixParam::OpType, i } }) != 0;
      bool sourceOn = hostContext->GetAudioParamList<int>({ modIndices, { (int)FFModMatrixParam::Source, i } }) != 0;
      bool targetOn = hostContext->GetAudioParamList<int>({ modIndices, { (int)FFModMatrixParam::Target, i } }) != 0;
      if (opTypeOn && sourceOn && targetOn)
      {
        amtNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::Amount, i } }));
        scaleNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::Scale, i } }));
        targetNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::Target, i } }));
        opTypeNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::OpType, i } }));
        sourceNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::Source, i } }));
      }
    }
    plugGUI->HostContext()->ClearModuleAudioParams(modIndices);
    for (int i = 0; i < amtNorm.size(); i++)
    {
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::Amount, i } }, amtNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::Scale, i } }, scaleNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::Target, i } }, targetNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::OpType, i } }, opTypeNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::Source, i } }, sourceNorm[i]);
    }
    auto const* slotsParam = hostContext->Topo()->audio.ParamAtTopo({ modIndices, { (int)FFModMatrixParam::Slots, 0 } });
    double slotsNorm = slotsParam->static_.Discrete().PlainToNormalizedFast((int)amtNorm.size());
    hostContext->PerformImmediateAudioParamEdit(slotsParam->runtimeParamIndex, slotsNorm);
  };

  auto reset = plugGUI->StoreComponent<FBAutoSizeButton>("Reset");
  grid->Add(0, 6, reset);
  reset->onClick = [plugGUI, global]() {
    int moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
    FBTopoIndices moduleIndices = { moduleType, 0 };
    std::string name = plugGUI->HostContext()->Topo()->ModuleAtTopo(moduleIndices)->name;
    plugGUI->HostContext()->UndoState().Snapshot("Reset " + name);
    plugGUI->HostContext()->ClearModuleAudioParams(moduleIndices);
  };

  grid->MarkSection({ { 0, 0 }, { 1, 7 } });
  return grid;
}
  
static Component*
MakeModMatrixSlotsGUI(bool global, int offset, FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> rowSizes(FFModMatrixGlobalMaxSlotCount + 1, 1);
  std::vector<int> columnSizes = { 0, 2, 2, 3, 0 };
  auto moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, rowSizes, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Op"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>("Source"));
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>("Scale"));
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>("Target"));
  grid->Add(0, 4, plugGUI->StoreComponent<FBAutoSizeLabel>("Amt"));
  for (int i = 0; i < FFModMatrixGlobalMaxSlotCount; i++)
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
  grid->MarkSection({ { 0, 0 }, { FFModMatrixGlobalMaxSlotCount + 1, (int)FFModMatrixParam::Count - 1 } });
  return grid;
}

static Component*
MakeModMatrixGUIVoice(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 0, 1 } }, std::vector<int> { { 1, 1 } });
  grid->Add(0, 0, 1, 2, MakeModMatrixTopGUI(false, plugGUI));
  grid->Add(1, 0, MakeModMatrixSlotsGUI(false, 0, plugGUI));
  grid->Add(1, 1, MakeModMatrixSlotsGUI(false, FFModMatrixGlobalMaxSlotCount, plugGUI));
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeModMatrixGUIGlobal(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 0, 1 } }, std::vector<int> { { 1 } });
  grid->Add(0, 0, MakeModMatrixTopGUI(true, plugGUI));
  grid->Add(1, 0, MakeModMatrixSlotsGUI(true, 0, plugGUI));
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

Component*
FFMakeModMatrixGUI(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 1 } }, std::vector<int> { { 2, 1 } });
  grid->Add(0, 0, MakeModMatrixGUIVoice(plugGUI));
  grid->Add(0, 1, MakeModMatrixGUIGlobal(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}