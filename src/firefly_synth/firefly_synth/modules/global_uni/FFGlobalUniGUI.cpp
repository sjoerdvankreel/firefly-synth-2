#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/graph/FBModuleGraphComponent.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FFGlobalUniParamListener::
~FFGlobalUniParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFGlobalUniParamListener::
FFGlobalUniParamListener(FBPlugGUI* plugGUI):
_plugGUI(plugGUI)
{
  _plugGUI->AddParamListener(this);
}

void 
FFGlobalUniParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool changedFromUI)
{
  if (!changedFromUI)
    return;
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  if (indices.module.index != (int)FFModuleType::GlobalUni)
    return;

  int target = -1;
  if (indices.param.index == (int)FFGlobalUniParam::Mode ||
    indices.param.index == (int)FFGlobalUniParam::OpType ||
    indices.param.index == (int)FFGlobalUniParam::AutoSpread ||
    indices.param.index == (int)FFGlobalUniParam::AutoSkew ||
    indices.param.index == (int)FFGlobalUniParam::AutoRand ||
    indices.param.index == (int)FFGlobalUniParam::AutoRandSeed ||
    indices.param.index == (int)FFGlobalUniParam::AutoRandFree)
    target = indices.param.slot;
  else if ((int)FFGlobalUniParam::ManualFirst <= indices.param.index && indices.param.index <= (int)FFGlobalUniParam::ManualLast)
    target = indices.param.index - (int)FFGlobalUniParam::ManualFirst;

  if (target == -1)
    return;
  FFModuleType targetModule = FFGlobalUniTargetToModule((FFGlobalUniTarget)target);
  dynamic_cast<FFPlugGUI&>(*_plugGUI).SwitchGraphsToModule((int)targetModule, 0);
}

static void
GlobalUniInit(FBPlugGUI* plugGUI)
{
  FBTopoIndices moduleIndices = { (int)FFModuleType::GlobalUni, 0 };
  std::string name = plugGUI->HostContext()->Topo()->ModuleAtTopo(moduleIndices)->name;
  plugGUI->HostContext()->UndoState().Snapshot("Init " + name);
  for (int s = 0; s < (int)FFGlobalUniTarget::Count; s++)
  {
    plugGUI->HostContext()->DefaultAudioParam({ { moduleIndices }, { (int)FFGlobalUniParam::Mode, s} });
    plugGUI->HostContext()->DefaultAudioParam({ { moduleIndices }, { (int)FFGlobalUniParam::OpType, s} });
  }
  for (int p = (int)FFGlobalUniParam::AutoFirst; p <= (int)FFGlobalUniParam::AutoLast; p++)
    for (int s = 0; s < (int)FFGlobalUniTarget::Count; s++)
      plugGUI->HostContext()->DefaultAudioParam({ { moduleIndices }, { p, s } });
  for (int p = (int)FFGlobalUniParam::ManualFirst; p <= (int)FFGlobalUniParam::ManualLast; p++)
    for (int s = 0; s < FFGlobalUniMaxCount; s++)
      plugGUI->HostContext()->DefaultAudioParam({ { moduleIndices }, { p, s } });
}

static Component*
MakeGlobalUniVoices(
  FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 0, 0, 0 });
  auto voiceCount = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Voices", FBLabelAlign::Left, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceCount, Slider::SliderStyle::RotaryVerticalDrag));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamDisplayLabel>(plugGUI, voiceCount));
  grid->MarkSection({ { 0, 0, }, { 1, 3 } }, FBGridSectionMark::DefaultBackground);
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}
 
static Component*
MakeGlobalUniInit(
  FBPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 1, 0 });
  auto initButton = plugGUI->StoreComponent<FBAutoSizeButton>(plugGUI, "Init");
  initButton->setTooltip("Set To Defaults");
  grid->Add(0, 1, initButton);
  initButton->onClick = [plugGUI]() { GlobalUniInit(plugGUI); };
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component* 
MakeGlobalUniContent(
  FBPlugGUI* plugGUI,
  FBGraphRenderState* graphRenderState, int offset,
  std::vector<FBModuleGraphComponent*>* fixedGraphs)
{
  auto rowSizes = std::vector<int>();
  rowSizes.push_back(1);
  int uniTargetCount = (int)FFGlobalUniTarget::Count;
  FB_ASSERT(uniTargetCount % 2 == 0);
  for (int i = 0; i < uniTargetCount / 2; i++)
    rowSizes.push_back(1);
  auto columnSizes = std::vector<int>();
  columnSizes.push_back(2);
  for(int i = 0; i < FFGlobalUniMaxCount + 7; i++)
    columnSizes.push_back(0);
  columnSizes.push_back(1);

  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, rowSizes, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Target"));
  auto mode0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, 0 } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, mode0->static_.display, FBLabelAlign::Left, FBLabelColors::PrimaryForeground));
  auto opType0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::OpType, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, opType0->static_.name, FBLabelAlign::Left, FBLabelColors::PrimaryForeground));
  grid->MarkSection({ { 0, 0 }, { 1, 1 } }, FBGridSectionMark::AlternateBackground);
  grid->MarkSection({ { 0, 1 }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);
  grid->MarkSection({ { 0, 2 }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);

  auto spread0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpread, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, spread0->static_.display));
  auto skew0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSkew, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, skew0->static_.display));
  auto rand0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRand, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, rand0->static_.display));
  auto seed0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandSeed, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, seed0->static_.display));
  auto free0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandFree, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, free0->static_.display));
  grid->MarkSection({ { 0, 3 }, { 1, 5 } }, FBGridSectionMark::AlternateBackground);

  for (int i = 0; i < FFGlobalUniMaxCount; i++)
    grid->Add(0, i + 8, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, std::to_string(i + 1), FBLabelAlign::Center));
  grid->MarkSection({ { 0, 8 }, { 1, FFGlobalUniMaxCount } }, FBGridSectionMark::AlternateBackground);

  grid->Add(0, 8 + FFGlobalUniMaxCount, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Out", FBLabelAlign::Left, FBLabelColors::PrimaryForeground));
  grid->MarkSection({ { 0, 8 + FFGlobalUniMaxCount }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);

  for (int r = 0; r < uniTargetCount / 2; r++)
  {
    int guiRow = r + 1;
    int targetIndex = (int)FFGlobalUniTargetByGUIOrder((FFGlobalUniTargetGUIOrder)(offset + r));
    auto targetName = FFGlobalUniTargetToString((FFGlobalUniTarget)targetIndex);
    grid->Add(guiRow, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, targetName));
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, targetIndex } });
    grid->Add(guiRow, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode, "", false));
    auto opType = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::OpType, targetIndex } });
    grid->Add(guiRow, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType, "", false));

    if (r % 2 == 1)
      grid->MarkSection({ { guiRow, 0 }, { 1, 3 } }, FBGridSectionMark::AlternateBackground);

    auto spread = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpread, targetIndex } });
    grid->Add(guiRow, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag, false));
    auto skew = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSkew, targetIndex } });
    grid->Add(guiRow, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skew, Slider::SliderStyle::RotaryVerticalDrag, false));
    auto random = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRand, targetIndex } });
    grid->Add(guiRow, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, random, Slider::SliderStyle::RotaryVerticalDrag, false));
    auto seed = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandSeed, targetIndex } });
    grid->Add(guiRow, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag, false));
    auto free = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandFree, targetIndex } });
    grid->Add(guiRow, 7, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, free, false));

    if (r % 2 == 1)
      grid->MarkSection({ { guiRow, 3 }, { 1, 5 } }, FBGridSectionMark::AlternateBackground);

    for (int p = 0; p < FFGlobalUniMaxCount; p++)
    {
      auto param = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::ManualFirst + targetIndex, p } });
      grid->Add(guiRow, p + 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, param, Slider::SliderStyle::RotaryVerticalDrag, false));
    }

    if(r %2 == 1)
      grid->MarkSection({ { guiRow, 8 }, { 1, FFGlobalUniMaxCount } }, FBGridSectionMark::AlternateBackground);

    int moduleRuntimeIndex = topo->moduleTopoToRuntime.at({ (int)FFModuleType::GlobalUni, 0 });
    auto uniGraph = plugGUI->StoreComponent<FBModuleGraphComponent>(plugGUI, false, graphRenderState, moduleRuntimeIndex, targetIndex, [plugGUI]() { return plugGUI->GetRenderType(true); });
    grid->Add(guiRow, 8 + FFGlobalUniMaxCount, uniGraph);
    fixedGraphs->push_back(uniGraph);
  }
 
  return grid;
}

Component*
FFMakeGlobalUniGUI(
  FBPlugGUI* plugGUI,
  FBGraphRenderState* graphRenderState,
  std::vector<FBModuleGraphComponent*>* fixedGraphs)
{  
  FB_LOG_ENTRY_EXIT();
  auto topGrid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, 0, -1, std::vector<int> { { 0 } }, std::vector<int> { { 0, 1 } });
  topGrid->Add(0, 0, MakeGlobalUniVoices(plugGUI));
  topGrid->Add(0, 1, MakeGlobalUniInit(plugGUI));
  auto topComponent = plugGUI->StoreComponent<FBThemedComponent>(plugGUI, (int)FFThemedComponentId::GlobalUniTop, topGrid);
  auto bottomGrid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 1, 1 } });
  bottomGrid->Add(0, 0, plugGUI->StoreComponent<FBMarginComponent>(
    plugGUI, false, true, false, false,
      plugGUI->StoreComponent<FBCardComponent>(
      plugGUI, MakeGlobalUniContent(plugGUI, graphRenderState, 0, fixedGraphs))));
  bottomGrid->Add(0, 1, plugGUI->StoreComponent<FBCardComponent>(
    plugGUI, MakeGlobalUniContent(plugGUI, graphRenderState, (int)FFGlobalUniTarget::Count / 2, fixedGraphs)));
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, false, -1, -1, std::vector<int> { { 0, 1 } }, std::vector<int> { { 1 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, true, false, true, topComponent));
  grid->Add(1, 0, plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, true, false, true, bottomGrid));
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::GlobalUni, 0, grid);
}

bool
FFGlobalUniAdjustParamModulationGUIBounds(
  FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm)
{
  // Better keep this in check with the processor's ApplyToVoice.

  int voiceCount = ctx->GetAudioParamDiscrete({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } });
  if (voiceCount < 2)
    return false;
  auto target = FFParamToGlobalUniTarget(ctx->Topo(), index);
  if (target == (FFGlobalUniTarget)-1)
    return false;
  auto mode = ctx->GetAudioParamList<FFGlobalUniMode>({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, (int)target}});
  if (mode == FFGlobalUniMode::Off)
    return false;
  auto opType = ctx->GetAudioParamList<FFModulationOpType>({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::OpType, (int)target} });
  if (opType == FFModulationOpType::Off)
    return false;
  
  // We don't need skewing or randomization to determine the outer bounds.
  float maxModSource = 0.0;
  float minModSource = 1.0f;
  float spread = (float)ctx->GetAudioParamIdentity({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpread, (int)target} });
  if (mode == FFGlobalUniMode::AutoLinear || mode == FFGlobalUniMode::AutoExp)
  {
    for (int voicePos = 0; voicePos <= 1; voicePos++)
    {
      if (FFModulationOpTypeSourceIsBipolar(opType))
      {
        minModSource = std::min(minModSource, 0.5f + (voicePos - 0.5f) * spread);
        maxModSource = std::max(maxModSource, 0.5f + (voicePos - 0.5f) * spread);
      }
      else
      {
        minModSource = std::min(minModSource, voicePos * spread);
        maxModSource = std::max(maxModSource, voicePos * spread);
      }
    }
  }
  else
  {
    int manualParamIndex = (int)FFGlobalUniParam::ManualFirst + (int)target;
    for (int voiceSlotInGroup = 0; voiceSlotInGroup < voiceCount; voiceSlotInGroup++)
    {
      auto const* paramTopo = ctx->Topo()->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { manualParamIndex, voiceSlotInGroup } });
      float modSource = (float)ctx->GetAudioParamNormalized(paramTopo->runtimeParamIndex);
      minModSource = std::min(modSource, minModSource);
      maxModSource = std::max(modSource, maxModSource);
    }
  }

  FFApplyGUIModulationBounds(opType, minModSource, maxModSource, 1.0f, currentMinNorm, currentMaxNorm);
  return true;
}