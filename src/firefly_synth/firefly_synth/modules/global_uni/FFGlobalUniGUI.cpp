#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBTopLevelEditor.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

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

// todo drop?
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
  dynamic_cast<FFPlugGUI&>(*_plugGUI).SwitchMainGraphToModule((int)targetModule, 0);
}

static Component*
MakeGlobalUniEditorHeader(
  FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 0, 0, 0 });
  auto voiceCount = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceCount));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceCount, Slider::SliderStyle::RotaryVerticalDrag));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamDisplayLabel>(plugGUI, voiceCount, std::to_string(FFGlobalUniMaxCount)));
  grid->MarkSection({ { 0, 0, }, { 1, 3 } });
  auto subSection = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(subSection);
}

static Component* 
MakeGlobalUniEditorContent(
  FBPlugGUI* plugGUI,
  FBGraphRenderState* graphRenderState,
  std::vector<FBModuleGraphComponent*>* fixedGraphs)
{
  FB_LOG_ENTRY_EXIT();
  auto rowSizes = std::vector<int>();
  rowSizes.push_back(1);
  int uniTargetCount = (int)FFGlobalUniTarget::Count;
  FB_ASSERT(uniTargetCount % 2 == 0);
  for (int i = 0; i < uniTargetCount / 2; i++)
    rowSizes.push_back(1);
  auto columnSizes = std::vector<int>();
  columnSizes.push_back(1);
  for(int i = 0; i < FFGlobalUniMaxCount + 8; i++)
    columnSizes.push_back(0);
  columnSizes.push_back(1);
  for (int i = 0; i < FFGlobalUniMaxCount + 8; i++)
    columnSizes.push_back(0);

  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, rowSizes, columnSizes);
  for (int c = 0; c < 2; c++)
  {
    int rowOffset = 0;
    int guiCol = c * (FFGlobalUniMaxCount + 8 + 1);
    grid->Add(rowOffset + 0, guiCol, plugGUI->StoreComponent<FBAutoSizeLabel>("Target"));
    grid->MarkSection({ { rowOffset + 0, guiCol }, { 1, 1 } });
    auto mode0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, 0 } });
    grid->Add(rowOffset + 0, guiCol + 1, plugGUI->StoreComponent<FBAutoSizeLabel>(mode0->static_.display));
    auto opType0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::OpType, 0 } });
    grid->Add(rowOffset + 0, guiCol + 2, plugGUI->StoreComponent<FBAutoSizeLabel>(opType0->static_.name));
    grid->Add(rowOffset + 0, guiCol + 3, plugGUI->StoreComponent<FBAutoSizeLabel>("Out"));
    grid->MarkSection({ { rowOffset + 0, guiCol + 1 }, { 1, 3 } });

    auto spread0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpread, 0 } });
    grid->Add(rowOffset + 0, guiCol + 4, plugGUI->StoreComponent<FBAutoSizeLabel>(spread0->static_.display));
    auto skew0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSkew, 0 } });
    grid->Add(rowOffset + 0, guiCol + 5, plugGUI->StoreComponent<FBAutoSizeLabel>(skew0->static_.display));
    auto rand0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRand, 0 } });
    grid->Add(rowOffset + 0, guiCol + 6, plugGUI->StoreComponent<FBAutoSizeLabel>(rand0->static_.display));
    auto seed0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandSeed, 0 } });
    grid->Add(rowOffset + 0, guiCol + 7, plugGUI->StoreComponent<FBAutoSizeLabel>(seed0->static_.display));
    auto free0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandFree, 0 } });
    grid->Add(rowOffset + 0, guiCol + 8, plugGUI->StoreComponent<FBAutoSizeLabel>(free0->static_.display));
    grid->MarkSection({ { rowOffset + 0, guiCol + 4 }, { 1, 5 } });

    for (int i = 0; i < FFGlobalUniMaxCount; i++)
      grid->Add(rowOffset + 0, guiCol + i + 1 + 8, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(i + 1), true));
    grid->MarkSection({ { rowOffset + 0, guiCol + 1 + 8 }, { 1, FFGlobalUniMaxCount } });
  }

  for (int c = 0; c < 2; c++)
  {
    for (int r = 0; r < uniTargetCount / 2; r++)
    {
      int guiRow = r + 1;
      int guiCol = c * (FFGlobalUniMaxCount + 1 + 8);
      int targetIndex = c * (uniTargetCount / 2) + r;

      auto targetName = FFGlobalUniTargetToString((FFGlobalUniTarget)targetIndex);
      grid->Add(guiRow, guiCol, plugGUI->StoreComponent<FBAutoSizeLabel>(targetName));
      grid->MarkSection({ { guiRow, guiCol }, { 1, 1 } });

      auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, targetIndex } });
      grid->Add(guiRow, guiCol + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
      auto opType = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::OpType, targetIndex } });
      grid->Add(guiRow, guiCol + 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));
      int moduleRuntimeIndex = topo->moduleTopoToRuntime.at({ (int)FFModuleType::GlobalUni, 0 });
      auto uniGraph = plugGUI->StoreComponent<FBModuleGraphComponent>(graphRenderState, moduleRuntimeIndex, targetIndex, [plugGUI]() { return plugGUI->GetGraphRenderType(); });
      grid->Add(guiRow, guiCol + 3, uniGraph);
      fixedGraphs->push_back(uniGraph);
      grid->MarkSection({ { guiRow, guiCol + 1 }, { 1, 3 } });

      auto spread = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpread, targetIndex } });
      grid->Add(guiRow, guiCol + 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag));
      auto skew = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSkew, targetIndex } });
      grid->Add(guiRow, guiCol + 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skew, Slider::SliderStyle::RotaryVerticalDrag));
      auto random = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRand, targetIndex } });
      grid->Add(guiRow, guiCol + 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, random, Slider::SliderStyle::RotaryVerticalDrag));
      auto seed = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandSeed, targetIndex } });
      grid->Add(guiRow, guiCol + 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));
      auto free = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandFree, targetIndex } });
      grid->Add(guiRow, guiCol + 8, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, free));
      grid->MarkSection({ { guiRow, guiCol + 4 }, { 1, 5 } });

      for (int p = 0; p < FFGlobalUniMaxCount; p++)
      {
        int manualParamOffset = c * uniTargetCount / 2 + r;
        auto param = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::ManualFirst + manualParamOffset, p } });
        grid->Add(guiRow, guiCol + p + 1 + 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, param, Slider::SliderStyle::RotaryVerticalDrag));
      }
      grid->MarkSection({ { guiRow, guiCol + 1 + 8 }, { 1, FFGlobalUniMaxCount } });
    }
  }

  return grid;
}

static Component*
MakeGlobalUniSectionMain(
  FBPlugGUI* plugGUI,
  FBGraphRenderState* graphRenderState,
  std::vector<FBModuleGraphComponent*>* fixedGraphs)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1 }, std::vector<int> { 0 });  
  auto editorHeader = MakeGlobalUniEditorHeader(plugGUI);
  auto editorContent = MakeGlobalUniEditorContent(plugGUI, graphRenderState, fixedGraphs);
  auto showEditor = plugGUI->StoreComponent<FBAutoSizeButton>("Edit");
  showEditor->onClick = [plugGUI, editorContent, editorHeader]() {
    FBTopLevelEditorParams params = {};
    params.w = 1200;
    params.h = 560;
    params.title = "Unison";
    params.iconFile = "header.png";
    params.header = editorHeader;
    params.content = editorContent;
    params.init = [plugGUI]() { GlobalUniInit(plugGUI); };
    // todo params.toggleModuleIndex = (int)FFModuleType::Settings;
    // params.toggleParamIndex = (int)FFSettingsGUIParam::ShowGlobalUni;
    plugGUI->OpenTopLevelEditor(FFTopLevelEditorGlobalUni, params);
  };
  grid->Add(0, 0, showEditor);
  grid->MarkSection({ { 0, 0 }, { 1, 1 } });
  auto subSection = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(subSection);
}

Component*
FFMakeGlobalUniGUI(
  FBPlugGUI* plugGUI,
  FBGraphRenderState* graphRenderState,
  std::vector<FBModuleGraphComponent*>* fixedGraphs)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::GlobalUni].name;
  tabs->addTab(name, {}, MakeGlobalUniSectionMain(plugGUI, graphRenderState, fixedGraphs), false);
  return tabs;
}