#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGUI.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGraph.hpp>

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
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBThemedComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

// +5 for "V"/"G"/+/-/up/down
static int const MatrixControlCount = 15;

FFModMatrixParamListener::
~FFModMatrixParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFModMatrixParamListener::
FFModMatrixParamListener(FBPlugGUI* plugGUI):
_plugGUI(plugGUI)
{
  _plugGUI->AddParamListener(this);
}

void 
FFModMatrixParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool changedFromUI)
{
  auto& ffGUI = dynamic_cast<FFPlugGUI&>(*_plugGUI);
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*_plugGUI->HostContext()->Topo()->static_.get());
  if (indices.module.index != (int)FFModuleType::VMatrix && indices.module.index != (int)FFModuleType::GMatrix)
    return;
  
  bool global = indices.module.index == (int)FFModuleType::GMatrix;
  auto const& sources = global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  auto const& targets = global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;

  if (changedFromUI)
  {
    if (indices.param.index == (int)FFModMatrixParam::Source ||
      indices.param.index == (int)FFModMatrixParam::SourceInv ||
      indices.param.index == (int)FFModMatrixParam::SourceLow ||
      indices.param.index == (int)FFModMatrixParam::SourceHigh)
    {
      auto sourceIndices = indices;
      sourceIndices.param.index = (int)FFModMatrixParam::Source;
      int sourceVal = _plugGUI->HostContext()->GetAudioParamList<int>(sourceIndices);
      auto const& moduleIndices = sources[sourceVal].indices.module;
      if (moduleIndices.index != -1)
        ffGUI.SwitchMainGraphToModule(moduleIndices.index, moduleIndices.slot);
    }

    if (indices.param.index == (int)FFModMatrixParam::Scale ||
      indices.param.index == (int)FFModMatrixParam::ScaleMin ||
      indices.param.index == (int)FFModMatrixParam::ScaleMax)
    {
      auto scaleIndices = indices;
      scaleIndices.param.index = (int)FFModMatrixParam::Scale;
      int scaleVal = _plugGUI->HostContext()->GetAudioParamList<int>(scaleIndices);
      auto const& moduleIndices = sources[scaleVal].indices.module;
      if (moduleIndices.index != -1)
        ffGUI.SwitchMainGraphToModule(moduleIndices.index, moduleIndices.slot);
    }

    if (indices.param.index == (int)FFModMatrixParam::Target ||
      indices.param.index == (int)FFModMatrixParam::TargetAmt)
    {
      auto targetIndices = indices;
      targetIndices.param.index = (int)FFModMatrixParam::Target;
      int targetVal = _plugGUI->HostContext()->GetAudioParamList<int>(targetIndices);
      auto const& moduleIndices = targets[targetVal].module;
      if (moduleIndices.index != -1)
        ffGUI.SwitchMainGraphToModule(moduleIndices.index, moduleIndices.slot);
    }
  }

  int maxSlotCount = global ? FFModMatrixGlobalMaxSlotCount : FFModMatrixVoiceMaxSlotCount;
  bool updateModHighlight = ffTopo.modules[indices.module.index].params[indices.param.index].slotCount == maxSlotCount;
  if (updateModHighlight)
  {
    auto targetIndices = indices;
    targetIndices.param.index = (int)FFModMatrixParam::Target;
    int targetVal = _plugGUI->HostContext()->GetAudioParamList<int>(targetIndices);
    if (targets[targetVal].module.index != -1)
      _plugGUI->RepaintSlidersForAudioParam(targets[targetVal]);
  }
}

static Component*
MakeModMatrixSlotControlGUI(bool global, FFPlugGUI* plugGUI)
{
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
  clean->setTooltip("Remove Inactive Slots");
  grid->Add(0, 5, clean);
  clean->onClick = [plugGUI, global]() {
    std::vector<double> scaleNorm = {};
    std::vector<double> sourceNorm = {};
    std::vector<double> targetNorm = {};
    std::vector<double> opTypeNorm = {};
    std::vector<double> scaleMinNorm = {};
    std::vector<double> scaleMaxNorm = {};
    std::vector<double> targetAmtNorm = {};
    std::vector<double> sourceInvNorm = {};
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
        targetAmtNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::TargetAmt, i } }));
        sourceInvNorm.push_back(hostContext->GetAudioParamNormalized({ modIndices, { (int)FFModMatrixParam::SourceInv, i } }));
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
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::TargetAmt, i } }, targetAmtNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::SourceInv, i } }, sourceInvNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::SourceLow, i } }, sourceLowNorm[i]);
      hostContext->PerformImmediateAudioParamEdit({ modIndices, { (int)FFModMatrixParam::SourceHigh, i } }, sourceHighNorm[i]);
    }
    auto const* slotsParam = hostContext->Topo()->audio.ParamAtTopo({ modIndices, { (int)FFModMatrixParam::Slots, 0 } });
    double slotsNorm = slotsParam->static_.Discrete().PlainToNormalizedFast((int)sourceNorm.size());
    hostContext->PerformImmediateAudioParamEdit(slotsParam->runtimeParamIndex, slotsNorm);
  };

  auto initButton = plugGUI->StoreComponent<FBAutoSizeButton>("Init");
  initButton->setTooltip("Set To Defaults");
  grid->Add(0, 6, initButton);
  initButton->onClick = [plugGUI, global]() {
    int moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);
    FBTopoIndices moduleIndices = { moduleType, 0 };
    std::string name = plugGUI->HostContext()->Topo()->ModuleAtTopo(moduleIndices)->name;
    plugGUI->HostContext()->UndoState().Snapshot("Init " + name);
    plugGUI->HostContext()->ClearModuleAudioParams(moduleIndices);
  };

  grid->MarkSection({ { 0, 0 }, { 1, 7 } }, FBGridSectionMark::BackgroundAndAlternate);
  return plugGUI->StoreComponent<FBMarginComponent>(true, true, false, true, grid);
}

static Component*
MakeModMatrixSlotControlGUI(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(false, std::vector<int> { { 1 } }, std::vector<int> { { 0, 0, 0 } });
  grid->Add(0, 0, MakeModMatrixSlotControlGUI(false, plugGUI));
  grid->Add(0, 1, MakeModMatrixSlotControlGUI(true, plugGUI));
  return grid;
}

static Component*
MakeModMatrixGraphGUI(FFPlugGUI* plugGUI, FFModMatrixGraphType type, bool last)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(false, std::vector<int> { { 1 } }, std::vector<int> { { 1 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FFModMatrixGraph>(plugGUI, type));
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(false, last, false, true, grid);
  return plugGUI->StoreComponent<FBThemedComponent>((int)FFThemedComponentId::MatrixGraphs, margin);
}

static Component*
MakeModMatrixTopGUI(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 1 } }, std::vector<int> { { 0, 1, 1, 1, 1, 1, 1 } });
  grid->Add(0, 0, MakeModMatrixSlotControlGUI(plugGUI));
  for(int i = 0; i < (int)FFModMatrixGraphType::Count; i++)
    grid->Add(0, 1 + i, MakeModMatrixGraphGUI(plugGUI, (FFModMatrixGraphType)i, i == (int)FFModMatrixGraphType::Count - 1));
  return grid;
}

static void
AddMatrixHeaderRow(FFPlugGUI* plugGUI, FBGridComponent* grid, int r, int c)
{
  grid->Add(r, c + 1, 1, 4, plugGUI->StoreComponent<FBAutoSizeLabel>("Order"));
  grid->Add(r, c + 5, plugGUI->StoreComponent<FBAutoSizeLabel>("Op"));
  grid->Add(r, c + 6, plugGUI->StoreComponent<FBAutoSizeLabel>("Source"));
  grid->Add(r, c + 7, plugGUI->StoreComponent<FBAutoSizeLabel>("Inv"));
  grid->Add(r, c + 8, plugGUI->StoreComponent<FBAutoSizeLabel>("Lo"));
  grid->Add(r, c + 9, plugGUI->StoreComponent<FBAutoSizeLabel>("Hi"));
  grid->Add(r, c + 10, plugGUI->StoreComponent<FBAutoSizeLabel>("Scale"));
  grid->Add(r, c + 11, plugGUI->StoreComponent<FBAutoSizeLabel>("Min"));
  grid->Add(r, c + 12, plugGUI->StoreComponent<FBAutoSizeLabel>("Max"));
  grid->Add(r, c + 13, plugGUI->StoreComponent<FBAutoSizeLabel>("Target"));
  grid->Add(r, c + 14, plugGUI->StoreComponent<FBAutoSizeLabel>("Amt"));;
}

static void
AddMatrixSlotRow(FFPlugGUI* plugGUI, FBGridComponent* grid, bool global, int r, int c, int slot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = (int)(global ? FFModuleType::GMatrix : FFModuleType::VMatrix);  
  int maxSlotCount = global ? FFModMatrixGlobalMaxSlotCount : FFModMatrixVoiceMaxSlotCount;
  auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::OpType, slot } });
  
  grid->Add(r, c + 0, plugGUI->StoreComponent<FBParamLinkedLabel>(plugGUI, opType, global ? "G" : "V"));

  auto addButton = plugGUI->StoreComponent<FBParamLinkedButton>(plugGUI, opType, "+");
  addButton->onClick = [plugGUI, slot, moduleType, maxSlotCount]() {
    plugGUI->HostContext()->UndoState().Snapshot("Add Matrix Row");
    for(int i = maxSlotCount; i > slot; i--)
      for(int p = 0; p < (int)FFModMatrixParam::Count; p++)
        if (plugGUI->HostContext()->Topo()->static_->modules[(int)moduleType].params[p].slotCount == maxSlotCount)
        {
          if(i < maxSlotCount)
            plugGUI->HostContext()->CopyAudioParam({ { (int)moduleType, 0 }, { p, i - 1} }, { { (int)moduleType, 0 }, { p, i } });
          plugGUI->HostContext()->DefaultAudioParam({ { (int)moduleType, 0 }, { p, i - 1} });
        }};
  grid->Add(r, c + 1, addButton);

  auto removeButton = plugGUI->StoreComponent<FBParamLinkedButton>(plugGUI, opType, "-");
  removeButton->onClick = [plugGUI, slot, moduleType, maxSlotCount]() {
    plugGUI->HostContext()->UndoState().Snapshot("Remove Matrix Row");
    for (int i = slot; i < maxSlotCount; i++)
      for (int p = 0; p < (int)FFModMatrixParam::Count; p++)
        if (plugGUI->HostContext()->Topo()->static_->modules[(int)moduleType].params[p].slotCount == maxSlotCount)
        {
          if(i == maxSlotCount - 1)
            plugGUI->HostContext()->DefaultAudioParam({ { (int)moduleType, 0 }, { p, i } });
          else
          {
            plugGUI->HostContext()->CopyAudioParam({ { (int)moduleType, 0 }, { p, i + 1 } }, { { (int)moduleType, 0 }, { p, i } });
            plugGUI->HostContext()->DefaultAudioParam({ { (int)moduleType, 0 }, { p, i + 1 } });
          }
        }};
  grid->Add(r, c + 2, removeButton);

  auto upButton = plugGUI->StoreComponent<FBParamLinkedButton>(plugGUI, opType, "\U00002191");
  upButton->onClick = [plugGUI, slot, moduleType, maxSlotCount]() {
    plugGUI->HostContext()->UndoState().Snapshot("Move Matrix Row Up");
    for (int p = 0; p < (int)FFModMatrixParam::Count; p++)
      if (plugGUI->HostContext()->Topo()->static_->modules[(int)moduleType].params[p].slotCount == maxSlotCount)
      {
        if (slot == 0)
        {
          plugGUI->HostContext()->DefaultAudioParam({ { (int)moduleType, 0 }, { p, 0 } });
          return;
        }
        double thisOne = plugGUI->HostContext()->GetAudioParamNormalized({ { (int)moduleType, 0 }, { p, slot } });
        double prevOne = plugGUI->HostContext()->GetAudioParamNormalized({ { (int)moduleType, 0 }, { p, slot - 1 } });
        plugGUI->HostContext()->PerformImmediateAudioParamEdit({ { (int)moduleType, 0 }, { p, slot } }, prevOne);
        plugGUI->HostContext()->PerformImmediateAudioParamEdit({ { (int)moduleType, 0 }, { p, slot - 1 } }, thisOne);
      }};
  grid->Add(r, c + 3, upButton);

  auto downButton = plugGUI->StoreComponent<FBParamLinkedButton>(plugGUI, opType, "\U00002193");
  downButton->onClick = [plugGUI, slot, moduleType, maxSlotCount]() {
    plugGUI->HostContext()->UndoState().Snapshot("Move Matrix Row Down");
    for (int p = 0; p < (int)FFModMatrixParam::Count; p++)
      if (plugGUI->HostContext()->Topo()->static_->modules[(int)moduleType].params[p].slotCount == maxSlotCount)
      {
        if (slot == maxSlotCount - 1)
        {
          plugGUI->HostContext()->DefaultAudioParam({ { (int)moduleType, 0 }, { p, slot } });
          return;
        }
        double thisOne = plugGUI->HostContext()->GetAudioParamNormalized({ { (int)moduleType, 0 }, { p, slot } });
        double nextOne = plugGUI->HostContext()->GetAudioParamNormalized({ { (int)moduleType, 0 }, { p, slot + 1 } });
        plugGUI->HostContext()->PerformImmediateAudioParamEdit({ { (int)moduleType, 0 }, { p, slot } }, nextOne);
        plugGUI->HostContext()->PerformImmediateAudioParamEdit({ { (int)moduleType, 0 }, { p, slot + 1 } }, thisOne);
      }};
  grid->Add(r, c + 4, downButton);

  grid->Add(r, c + 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));

  std::function<void(int)> sourceOrScaleChanged = [plugGUI, global](int itemResultId) {
    if (itemResultId != 0)
    {
      auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*plugGUI->HostContext()->Topo()->static_);
      auto const& sources = global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
      auto const& moduleIndices = sources[itemResultId - 1].indices.module;
      if (moduleIndices.index != -1)
        plugGUI->SwitchMainGraphToModule(moduleIndices.index, moduleIndices.slot);
    } };

  auto source = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Source, slot } });
  auto* sourceCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, source);
  sourceCombo->valueChangedByUserAction = sourceOrScaleChanged;
  grid->Add(r, c + 6, sourceCombo);
  auto sourceInv = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::SourceInv, slot } });
  grid->Add(r, c + 7, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sourceInv));
  auto sourceLow = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::SourceLow, slot } });
  grid->Add(r, c + 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, sourceLow, Slider::SliderStyle::RotaryVerticalDrag));
  auto sourceHigh = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::SourceHigh, slot } });
  grid->Add(r, c + 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, sourceHigh, Slider::SliderStyle::RotaryVerticalDrag));

  auto scale = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Scale, slot } });
  auto* scaleCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, scale);
  scaleCombo->valueChangedByUserAction = sourceOrScaleChanged;
  grid->Add(r, c + 10, scaleCombo);
  auto scaleMin = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::ScaleMin, slot } });
  grid->Add(r, c + 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, scaleMin, Slider::SliderStyle::RotaryVerticalDrag));
  auto scaleMax = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::ScaleMax, slot } });
  grid->Add(r, c + 12, plugGUI->StoreComponent<FBParamSlider>(plugGUI, scaleMax, Slider::SliderStyle::RotaryVerticalDrag));

  auto target = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::Target, slot } });
  auto* targetCombo = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target);
  targetCombo->valueChangedByUserAction = [plugGUI, global](int itemResultId) {
    if (itemResultId != 0)
    {
      auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*plugGUI->HostContext()->Topo()->static_);
      auto const& targets = global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;
      auto const& moduleIndices = targets[itemResultId - 1].module;
      if (moduleIndices.index != -1)
        plugGUI->SwitchMainGraphToModule(moduleIndices.index, moduleIndices.slot);
    } };
  grid->Add(r, c + 13, targetCombo);
  auto targetAmt = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFModMatrixParam::TargetAmt, slot } });
  grid->Add(r, c + 14, plugGUI->StoreComponent<FBParamSlider>(plugGUI, targetAmt, Slider::SliderStyle::RotaryVerticalDrag));;
}
  
static Component*
MakeModMatrixSlotsGUI(FFPlugGUI* plugGUI, bool global, int offset, int count)
{
  // + 1 for header
  int rowCount = count + 1;
  std::vector<int> rowSizes(rowCount, 1);
  std::vector<int> autoSizeColForRow(rowSizes.size(), -1);
  std::vector<int> columnSizes = { 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 4, 0 };
  std::vector<int> autoSizeRowForCol = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, autoSizeRowForCol, autoSizeColForRow, rowSizes, columnSizes);

  AddMatrixHeaderRow(plugGUI, grid, 0, 0);
  for (int r = 0; r < count; r++)
    AddMatrixSlotRow(plugGUI, grid, global, 1 + r, 0, r + offset);

  for(int r = 0; r < rowCount; r++)
  {
    grid->MarkSection({ { r, 0 }, { 1, 5 } }, FBGridSectionMark::Background);
    grid->MarkSection({ { r, 5 }, { 1, 1 } }, FBGridSectionMark::Background);
    grid->MarkSection({ { r, 6 }, { 1, 4 } }, FBGridSectionMark::Background);
    grid->MarkSection({ { r, 10 }, { 1, 3 } }, FBGridSectionMark::Background);
    grid->MarkSection({ { r, 13 }, { 1, 2 } }, FBGridSectionMark::Background, 3.0f, r == rowCount - 1? 2: 0);
  }

  return grid;
}

static Component*
MakeModMatrixAllSlotsGUI(FFPlugGUI* plugGUI)
{
  auto mainGrid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 1 } }, std::vector<int> { { 1, 1 } });
  auto leftGrid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 1 } }, std::vector<int> { { 1 } });
  leftGrid->Add(0, 0, 1, 1, MakeModMatrixSlotsGUI(plugGUI, false, 0, 19));
  mainGrid->Add(0, 0, leftGrid);
  auto rightGrid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 7, 13 } }, std::vector<int> { { 1 } });
  rightGrid->Add(0, 0, MakeModMatrixSlotsGUI(plugGUI, false, 19, FFModMatrixVoiceMaxSlotCount - 19));
  rightGrid->Add(1, 0, MakeModMatrixSlotsGUI(plugGUI, true, 0, FFModMatrixGlobalMaxSlotCount));
  mainGrid->Add(0, 1, rightGrid);
  auto innerMargin = plugGUI->StoreComponent<FBMarginComponent>(true, true, true, true, mainGrid);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 1 } }, std::vector<int> { { 1 } });
  grid->Add(0, 0, innerMargin);
  grid->MarkSection({ { 0, 0 }, { 1, 1 } }, FBGridSectionMark::Alternate); // todo
  return plugGUI->StoreComponent<FBMarginComponent>(true, true, true, true, grid);
}

Component*
FFMakeModMatrixGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 0, 1 } }, std::vector<int> { { 1 } });
  grid->Add(0, 0, MakeModMatrixTopGUI(plugGUI));
  grid->Add(1, 0, MakeModMatrixAllSlotsGUI(plugGUI));
  return grid;
}

bool
FFModMatrixAdjustParamModulationGUIBounds(
  FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm)
{
  // Figure out maximum bounds of what the mod matrix would do to the 
  // current value of all sources/scales would use the full 0..1 range.

  bool haveAny = false;
  int rtIndex = ctx->Topo()->audio.params[index].runtimeModuleIndex;
  int staticIndex = ctx->Topo()->modules[rtIndex].topoIndices.index;
  bool isGlobal = !ctx->Topo()->static_->modules[staticIndex].voice;
  auto const& ffTopo = static_cast<FFStaticTopo const&>(*ctx->Topo()->static_);

  int moduleType = (int)(isGlobal ? FFModuleType::GMatrix : FFModuleType::VMatrix);
  auto const& targets = isGlobal ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;
  auto const& sources = isGlobal ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  int slotCount = ctx->GetAudioParamDiscrete({ { moduleType, 0, }, { (int)FFModMatrixParam::Slots, 0 } });

  for (int s = 0; s < slotCount; s++)
  {
    int source = ctx->GetAudioParamList<int>({ { moduleType, 0, }, { (int)FFModMatrixParam::Source, s } });
    int target = ctx->GetAudioParamList<int>({ { moduleType, 0, }, { (int)FFModMatrixParam::Target, s } });
    if(sources[source].indices.module.index != -1 && targets[target].module.index != -1)
      if (ctx->Topo()->audio.ParamAtTopo(targets[target])->runtimeParamIndex == index)
      {
        auto opType = ctx->GetAudioParamList<FFModulationOpType>({ { moduleType, 0, }, { (int)FFModMatrixParam::OpType, s } });
        if (opType != FFModulationOpType::Off)
        {
          float scaleMin = (float)ctx->GetAudioParamNormalized({ { moduleType, 0, }, { (int)FFModMatrixParam::ScaleMin, s } });
          float scaleMax = (float)ctx->GetAudioParamNormalized({ { moduleType, 0, }, { (int)FFModMatrixParam::ScaleMax, s } });
          float targetAmt = (float)ctx->GetAudioParamNormalized({ { moduleType, 0, }, { (int)FFModMatrixParam::TargetAmt, s } });
          FFApplyGUIModulationBounds(opType, 0.0f, 1.0f, scaleMin * targetAmt, currentMinNorm, currentMaxNorm);
          FFApplyGUIModulationBounds(opType, 0.0f, 1.0f, scaleMax * targetAmt, currentMinNorm, currentMaxNorm);
          haveAny = true;
        }
      }
  }

  return haveAny;
}