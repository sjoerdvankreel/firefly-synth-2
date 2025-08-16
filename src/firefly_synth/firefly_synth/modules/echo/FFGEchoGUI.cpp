#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/echo/FFGEchoGUI.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBMultiLineLabel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component*
MakeGEchoSectionMain(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto target = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::Target, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, target));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target));
  auto guiTapSelect = topo->gui.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoGUIParam::TapSelect, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, guiTapSelect));
  grid->Add(0, 3, plugGUI->StoreComponent<FBGUIParamSlider>(plugGUI, guiTapSelect, Slider::SliderStyle::RotaryVerticalDrag));
  auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::Mix, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  auto sync = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::Sync, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}