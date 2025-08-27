#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

Component*
FFMakeGUISettingsGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto showMatrix = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::ShowMatrix, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, showMatrix));
  auto showMatrixToggle = plugGUI->StoreComponent<FBGUIParamToggleButton>(plugGUI, showMatrix);
  showMatrixToggle->onClick = [plugGUI, showMatrixToggle]() { dynamic_cast<FFPlugGUI&>(*plugGUI).ToggleMatrix(showMatrixToggle->getToggleState()); };
  grid->Add(0, 1, showMatrixToggle);
  auto graphRenderMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::GraphRenderMode, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, graphRenderMode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, graphRenderMode));
  auto knobRenderMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::KnobRenderMode, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, knobRenderMode));
  grid->Add(0, 5, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, knobRenderMode));
  auto flushDelayButton = plugGUI->StoreComponent<FBAutoSizeButton>("Flush Audio");
  flushDelayButton->setTooltip("Flush delays and reverbs");
  grid->Add(0, 6, flushDelayButton);
  flushDelayButton->onClick = [plugGUI]() {
    FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsParam::FlushDelayToggle, 0 } };
    double flushNorm = plugGUI->HostContext()->GetAudioParamNormalized(indices);
    double newFlushNorm = flushNorm > 0.5 ? 0.0 : 1.0;
    plugGUI->HostContext()->PerformImmediateAudioParamEdit(indices, newFlushNorm);
  };
  grid->Add(0, 7, plugGUI->StoreComponent<FBFillerComponent>(10, 1));
  grid->MarkSection({ { 0, 0 }, { 1, 8 } });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
}