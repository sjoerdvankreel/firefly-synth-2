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
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component* 
MakeGlobalUniFullEditor(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto rowSizes = std::vector<int>();
  rowSizes.push_back(1);
  int uniControlCount = (int)FFGlobalUniParam::FullLast - (int)FFGlobalUniParam::FullFirst + 1;
  FB_ASSERT(uniControlCount % 2 == 0);
  for (int i = 0; i < uniControlCount / 2; i++)
    rowSizes.push_back(1);
  auto columnSizes = std::vector<int>();
  columnSizes.push_back(1);
  for(int i = 0; i < FFGlobalUniMaxCount; i++)
    columnSizes.push_back(0);
  columnSizes.push_back(1);
  for (int i = 0; i < FFGlobalUniMaxCount; i++)
    columnSizes.push_back(0);

  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, rowSizes, columnSizes);
  for (int c = 0; c < 2; c++)
  {
    int guiCol = c * (FFGlobalUniMaxCount + 1);
    grid->Add(0, guiCol, plugGUI->StoreComponent<FBAutoSizeLabel>("Param/Voice"));
    grid->MarkSection({ { 0, guiCol }, { 1, 1 } });
    for (int i = 0; i < FFGlobalUniMaxCount; i++)
    {
      grid->Add(0, guiCol + i + 1, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(i + 1), true));
      grid->MarkSection({ { 0, guiCol + i + 1 }, { 1, 1 } });
    }
  }

  for (int c = 0; c < 2; c++)
  {
    for (int r = 0; r < uniControlCount / 2; r++)
    {
      int guiRow = r + 1;
      int guiCol = c * (FFGlobalUniMaxCount + 1);
      int paramOffset = c * uniControlCount / 2 + r;
      auto param0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::FullFirst + paramOffset, 0 } });
      grid->Add(guiRow, guiCol, plugGUI->StoreComponent<FBParamLinkedLabel>(plugGUI, param0, param0->static_.display));
      grid->MarkSection({ { guiRow, guiCol }, { 1, 1 } });
      for (int p = 0; p < FFGlobalUniMaxCount; p++)
      {
        auto param = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::FullFirst + paramOffset, p } });
        grid->Add(guiRow, guiCol + p + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, param, Slider::SliderStyle::RotaryVerticalDrag));
        grid->MarkSection({ { guiRow, guiCol + p + 1 }, { 1, 1 } });
      }
    }
  }
  return grid;
}

static Component*
MakeGlobalUniSectionMain(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0 });
  
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto voiceCount = topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceCount, Slider::SliderStyle::RotaryVerticalDrag));

  auto fullEditor = MakeGlobalUniFullEditor(plugGUI);
  auto showFullEditor = plugGUI->StoreComponent<FBAutoSizeButton>("Editor");
  showFullEditor->onClick = [plugGUI, fullEditor]() {
    dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent("Global Unison", fullEditor, 640, 450, [plugGUI]() {
      FBTopoIndices moduleIndices = { (int)FFModuleType::GlobalUni, 0 };
      std::string name = plugGUI->HostContext()->Topo()->ModuleAtTopo(moduleIndices)->name;
      plugGUI->HostContext()->UndoState().Snapshot("Init " + name);
      for (int p = (int)FFGlobalUniParam::FullFirst; p <= (int)FFGlobalUniParam::FullLast; p++)
        for (int s = 0; s < FFGlobalUniMaxCount; s++)
          plugGUI->HostContext()->DefaultAudioParam({ { moduleIndices }, { p, s } });
      });
    };
  grid->Add(1, 0, 1, 3, showFullEditor);
  grid->MarkSection({ { 0, 0 }, { 2, 3 } });
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