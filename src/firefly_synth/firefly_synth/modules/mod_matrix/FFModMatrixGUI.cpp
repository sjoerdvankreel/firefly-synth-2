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
MakeModMatrixSlotControlGUI(bool global, FFPlugGUI* plugGUI)
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
    std::vector<double> scaleNorm = {};
    std::vector<double> sourceNorm = {};
    std::vector<double> targetNorm = {};
    std::vector<double> opTypeNorm = {};
    std::vector<double> scaleMinNorm = {};
    std::vector<double> scaleMaxNorm = {};
    std::vector<double> targetMinNorm = {};
    std::vector<double> targetMaxNorm = {};
    std::vector<double> sourceLowNorm = {};
    std::vector<double> sourceHighNorm = {};
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
        scaleNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::Scale, i } }));
        targetNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::Target, i } }));
        opTypeNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::OpType, i } }));
        sourceNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::Source, i } }));
        scaleMinNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::ScaleMin, i } }));
        scaleMaxNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::ScaleMax, i } }));
        targetMinNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::TargetMin, i } }));
        targetMaxNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::TargetMax, i } }));
        sourceLowNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::SourceLow, i } }));
        sourceHighNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::SourceHigh, i } }));
      }
    }
    plugGUI->HostContext()->ClearModuleAudioParams(modIndices);
    for (int i = 0; i < sourceNorm.size(); i++)
    {
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::Scale, i } }, scaleNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::Target, i } }, targetNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::OpType, i } }, opTypeNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::Source, i } }, sourceNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::ScaleMin, i } }, scaleMinNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::ScaleMax, i } }, scaleMaxNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::TargetMin, i } }, targetMinNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::TargetMax, i } }, targetMaxNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::SourceLow, i } }, sourceLowNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::SourceHigh, i } }, sourceHighNorm[i]);
    }
    auto const* slotsParam = hostContext->Topo()->audio.ParamAtTopo({ modIndices, { (int)FFModMatrixParam::Slots, 0 } });
    double slotsNorm = slotsParam->static_.Discrete().PlainToNormalizedFast((int)sourceNorm.size());
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
MakeModMatrixTopGUI(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 1 } }, std::vector<int> { { 0, 0, 1 } });
  grid->Add(0, 0, MakeModMatrixSlotControlGUI(true, plugGUI));
  grid->Add(0, 1, MakeModMatrixSlotControlGUI(false, plugGUI));
  grid->MarkSection({ { 0, 0 }, { 1, 1 } });
  grid->MarkSection({ { 0, 1 }, { 1, 1 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static void
AddMatrixSlotRow(FFPlugGUI* plugGUI, FBGridComponent* grid, bool global, int r, int c, int slot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
  
  grid->Add(r, c + 0, plugGUI->StoreComponent<FBAutoSizeLabel>(global ? "G" : "V"));
  auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, slot } });
  grid->Add(r, c + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));

  std::function<void(int)> sourceOrScaleChanged = [plugGUI, global](int itemResultId) {
    if (itemResultId != 0)
    {
      auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*plugGUI->HostContext()->Topo()->static_);
      auto const& sources = global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
      auto const& moduleIndices = sources[itemResultId - 1].indices.module;
      if (moduleIndices.index != -1)
        plugGUI->SwitchGraphToModule(moduleIndices.index, moduleIndices.slot);
    } };

  auto source = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, slot } });
  auto* sourceCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, source);
  sourceCombo->valueChangedByUserAction = sourceOrScaleChanged;
  grid->Add(r, c + 2, sourceCombo);

  auto scale = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Scale, slot } });
  auto* scaleCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, scale);
  scaleCombo->valueChangedByUserAction = sourceOrScaleChanged;
  grid->Add(r, c + 3, scaleCombo);

  auto target = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, slot } });
  auto* targetCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target);
  targetCombo->valueChangedByUserAction = [plugGUI, global](int itemResultId) {
    if (itemResultId != 0)
    {
      auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*plugGUI->HostContext()->Topo()->static_);
      auto const& targets = global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;
      auto const& moduleIndices = targets[itemResultId - 1].module;
      if (moduleIndices.index != -1)
        plugGUI->SwitchGraphToModule(moduleIndices.index, moduleIndices.slot);
    } };

  grid->Add(r, c + 4, targetCombo);

  auto amount = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Amount, slot } });
  grid->Add(r, c + 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));
}
  
static Component*
MakeModMatrixSlotsGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  int controlCount = 6;
  int rowCount = (FFModMatrixGlobalMaxSlotCount + FFModMatrixVoiceMaxSlotCount) / 2;
  std::vector<int> columnSizes = { 0, 0, 2, 2, 3, 0, 0, 0, 2, 2, 3, 0 };
  std::vector<int> rowSizes(rowCount + 1, 1);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, rowSizes, columnSizes);

  for (int c = 0; c < 2; c++)
  {
    grid->Add(0, c * controlCount + 1, plugGUI->StoreComponent<FBAutoSizeLabel>("Op"));
    grid->Add(0, c * controlCount + 2, plugGUI->StoreComponent<FBAutoSizeLabel>("Source"));
    grid->Add(0, c * controlCount + 3, plugGUI->StoreComponent<FBAutoSizeLabel>("Scale"));
    grid->Add(0, c * controlCount + 4, plugGUI->StoreComponent<FBAutoSizeLabel>("Target"));
    grid->Add(0, c * controlCount + 5, plugGUI->StoreComponent<FBAutoSizeLabel>("Amt"));
  }

  for (int r = 0; r < FFModMatrixGlobalMaxSlotCount; r++)
    AddMatrixSlotRow(plugGUI, grid, true, 1 + r, 0, r);
  for (int r = FFModMatrixGlobalMaxSlotCount; r < rowCount; r++)
    AddMatrixSlotRow(plugGUI, grid, false, 1 + r, 0, r - FFModMatrixGlobalMaxSlotCount);
  for (int r = 0; r < rowCount; r++)
    AddMatrixSlotRow(plugGUI, grid, false, 1 + r, controlCount, r + rowCount - FFModMatrixGlobalMaxSlotCount);

  grid->MarkSection({ { 0, 0 }, { 1, controlCount } });
  grid->MarkSection({ { 0, controlCount }, { 1, controlCount } });
  grid->MarkSection({ { 1, controlCount }, { rowCount, controlCount } });
  grid->MarkSection({ { 1, 0 }, { FFModMatrixGlobalMaxSlotCount, controlCount } });
  grid->MarkSection({ { 1 + FFModMatrixGlobalMaxSlotCount, 0 }, { rowCount - FFModMatrixGlobalMaxSlotCount, controlCount } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

Component*
FFMakeModMatrixGUI(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 0, 1 } }, std::vector<int> { { 1 } });
  grid->Add(0, 0, MakeModMatrixTopGUI(plugGUI));
  grid->Add(1, 0, MakeModMatrixSlotsGUI(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}