#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component* 
MakeGlobalUniEditor(FBPlugGUI* plugGUI)
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
  for(int i = 0; i < FFGlobalUniMaxCount + 6; i++)
    columnSizes.push_back(0);
  columnSizes.push_back(1);
  for (int i = 0; i < FFGlobalUniMaxCount + 6; i++)
    columnSizes.push_back(0);

  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, rowSizes, columnSizes);
  for (int c = 0; c < 2; c++)
  {
    int guiCol = c * (FFGlobalUniMaxCount + 6 + 1);
    grid->Add(0, guiCol, plugGUI->StoreComponent<FBAutoSizeLabel>("Target"));
    grid->MarkSection({ { 0, guiCol }, { 1, 1 } });
    auto mode0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, 0 } });
    grid->Add(0, guiCol + 1, plugGUI->StoreComponent<FBAutoSizeLabel>(mode0->static_.name));
    grid->MarkSection({ { 0, guiCol + 1 }, { 1, 1 } });

    auto spread0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpread, 0 } });
    grid->Add(0, guiCol + 2, plugGUI->StoreComponent<FBAutoSizeLabel>(spread0->static_.name));
    auto space0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpace, 0 } });
    grid->Add(0, guiCol + 3, plugGUI->StoreComponent<FBAutoSizeLabel>(space0->static_.name));
    auto rand0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRand, 0 } });
    grid->Add(0, guiCol + 4, plugGUI->StoreComponent<FBAutoSizeLabel>(rand0->static_.name));
    auto seed0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandSeed, 0 } });
    grid->Add(0, guiCol + 5, plugGUI->StoreComponent<FBAutoSizeLabel>(seed0->static_.name));
    auto free0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandFree, 0 } });
    grid->Add(0, guiCol + 6, plugGUI->StoreComponent<FBAutoSizeLabel>(free0->static_.name));
    grid->MarkSection({ { 0, guiCol + 2 }, { 1, 5 } });

    for (int i = 0; i < FFGlobalUniMaxCount; i++)
      grid->Add(0, guiCol + i + 1 + 6, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(i + 1), true));
    grid->MarkSection({ { 0, guiCol + 1 + 6 }, { 1, FFGlobalUniMaxCount } });
  }

  for (int c = 0; c < 2; c++)
  {
    for (int r = 0; r < uniTargetCount / 2; r++)
    {
      int guiRow = r + 1;
      int guiCol = c * (FFGlobalUniMaxCount + 1 + 6);
      int targetIndex = c * (uniTargetCount / 2) + r;

      auto targetName = FFGlobalUniTargetToString((FFGlobalUniTarget)targetIndex);
      grid->Add(guiRow, guiCol, plugGUI->StoreComponent<FBAutoSizeLabel>(targetName));
      grid->MarkSection({ { guiRow, guiCol }, { 1, 1 } });

      auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, targetIndex } });
      grid->Add(guiRow, guiCol + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
      grid->MarkSection({ { guiRow, guiCol + 1 }, { 1, 1 } });

      auto spread = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpread, targetIndex } });
      grid->Add(guiRow, guiCol + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag));
      auto space = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoSpace, targetIndex } });
      grid->Add(guiRow, guiCol + 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, space, Slider::SliderStyle::RotaryVerticalDrag));
      auto random = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRand, targetIndex } });
      grid->Add(guiRow, guiCol + 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, random, Slider::SliderStyle::RotaryVerticalDrag));
      auto seed = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandSeed, targetIndex } });
      grid->Add(guiRow, guiCol + 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));
      auto free = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::AutoRandFree, targetIndex } });
      grid->Add(guiRow, guiCol + 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, free));
      grid->MarkSection({ { guiRow, guiCol + 2 }, { 1, 5 } });

      for (int p = 0; p < FFGlobalUniMaxCount; p++)
      {
        int manualParamOffset = c * uniTargetCount / 2 + r;
        auto param = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::ManualFirst + manualParamOffset, p } });
        grid->Add(guiRow, guiCol + p + 1 + 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, param, Slider::SliderStyle::RotaryVerticalDrag));
      }
      grid->MarkSection({ { guiRow, guiCol + 1 + 6 }, { 1, FFGlobalUniMaxCount } });
    }
  }
  return grid;
}

static Component*
MakeGlobalUniSectionMain(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0 });
  
  auto voiceCount = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceCount));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceCount, Slider::SliderStyle::RotaryVerticalDrag));

  auto editor = MakeGlobalUniEditor(plugGUI);
  auto showEditor = plugGUI->StoreComponent<FBParamLinkedButton>(plugGUI, voiceCount, "Edit");
  showEditor->onClick = [plugGUI, editor]() {
    dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent("Global Unison", editor, 1080, 450, [plugGUI]() {
      FBTopoIndices moduleIndices = { (int)FFModuleType::GlobalUni, 0 };
      std::string name = plugGUI->HostContext()->Topo()->ModuleAtTopo(moduleIndices)->name;
      plugGUI->HostContext()->UndoState().Snapshot("Init " + name);

      // TODO
      for (int p = (int)FFGlobalUniParam::ManualFirst; p <= (int)FFGlobalUniParam::ManualLast; p++)
        for (int s = 0; s < FFGlobalUniMaxCount; s++)
          plugGUI->HostContext()->DefaultAudioParam({ { moduleIndices }, { p, s } });
      });
    };
  grid->Add(1, 0, 1, 2, showEditor);
  grid->MarkSection({ { 0, 0 }, { 2, 2 } });
  auto subSection = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(subSection);
}

Component*
FFMakeGlobalUniGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::GlobalUni].name;
  tabs->addTab(name, {}, MakeGlobalUniSectionMain(plugGUI), false);
  return tabs;
}