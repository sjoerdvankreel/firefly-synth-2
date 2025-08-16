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

static Component*
MakeGEchoSectionTap(FBPlugGUI* plugGUI, int tap)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto tapOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapOn, tap } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapOn));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapOn));
  auto tapFeedback = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFeedback, tap } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFeedback));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapFeedback, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapBalance = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapBalance, tap } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapBalance));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapBalance, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapLevel = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLevel, tap } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLevel));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLevel, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapXOver = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapXOver, tap } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapXOver));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapXOver, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapFBXOver = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFBXOver, tap } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFBXOver));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapFBXOver, Slider::SliderStyle::RotaryVerticalDrag));

  auto tapLpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLPOn, tap } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpOn));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapLpOn));
  auto tapLpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLPFreq, tap } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpFreq));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapLpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLPRes, tap } });
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpRes));
  grid->Add(0, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapHpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapHPOn, tap } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpOn));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapHpOn));
  auto tapHpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapHPFreq, tap } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpFreq));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapHpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapHPRes, tap } });
  grid->Add(1, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpRes));
  grid->Add(1, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpRes, Slider::SliderStyle::RotaryVerticalDrag));

  auto tapFBLpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFBLPOn, tap } });
  grid->Add(0, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFBLpOn));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapFBLpOn));
  auto tapFBLpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFBLPFreq, tap } });
  grid->Add(0, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFBLpFreq));
  grid->Add(0, 15, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapFBLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapFBLpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFBLPRes, tap } });
  grid->Add(0, 16, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFBLpRes));
  grid->Add(0, 17, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapFBLpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapFBHpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFBHPOn, tap } });
  grid->Add(1, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFBHpOn));
  grid->Add(1, 13, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapFBHpOn));
  auto tapFBHpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFBHPFreq, tap } });
  grid->Add(1, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFBHpFreq));
  grid->Add(1, 15, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapFBHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapFBHpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapFBHPRes, tap } });
  grid->Add(1, 16, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapFBHpRes));
  grid->Add(1, 17, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapFBHpRes, Slider::SliderStyle::RotaryVerticalDrag));

  auto tapDelayTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapDelayTime, tap } });
  grid->Add(0, 18, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayTime));
  grid->Add(0, 19, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapDelayTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapDelayBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapDelayBars, tap } });
  grid->Add(0, 18, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayBars));
  grid->Add(0, 19, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tapDelayBars));
  auto tapLengthTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLengthTime, tap } });
  grid->Add(1, 18, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLengthTime));
  grid->Add(1, 19, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLengthTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapLengthBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLengthBars, tap } });
  grid->Add(1, 18, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLengthBars));
  grid->Add(1, 19, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tapLengthBars));

  grid->MarkSection({ { 0, 0 }, { 2, 20 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}