#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoGUI.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBMultiLineLabel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBContentComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component* 
MakeEchoTapsEditor(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto topo = plugGUI->HostContext()->Topo();
  auto rowSizes = std::vector<int>();
  rowSizes.push_back(1);
  for (int i = 0; i < FFEchoTapCount; i++)
    rowSizes.push_back(1);
  auto columnSizes = std::vector<int> { { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 } };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, rowSizes, columnSizes);

  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>("On"));
  auto tapDelayLevel0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLevel, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayLevel0));
  auto tapDelayTime0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayTime, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayTime0));
  auto tapDelayBars0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayBars, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayBars0));
  auto tapBalance0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapBalance, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapBalance0));
  auto tapLpFreq0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPFreq, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpFreq0));
  auto tapLpRes0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPRes, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpRes0));
  auto tapHpFreq0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPFreq, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpFreq0));
  auto tapHpRes0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPRes, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpRes0));
  auto tapXOver0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapXOver, 0 } });
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapXOver0));

  for(int i = 0; i < 9; i++)
    grid->MarkSection({ { 0, 1 + i }, { 1, 1 } });  

  for (int t = 0; t < FFEchoTapCount; t++)
  {
    grid->Add(t + 1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(t + 1)));
    auto tapOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapOn, t } });
    grid->Add(1 + t, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapOn));
    auto tapLevel = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLevel, t } });
    grid->Add(1 + t, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLevel, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapDelayTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayTime, t } });
    grid->Add(1 + t, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapDelayTime, Slider::SliderStyle::LinearHorizontal));
    auto tapDelayBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayBars, t } });
    grid->Add(1 + t, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tapDelayBars));
    auto tapBalance = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapBalance, t } });
    grid->Add(1 + t, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapBalance, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapLpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPFreq, t } });
    grid->Add(1 + t, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapLpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPRes, t } });
    grid->Add(1 + t, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpRes, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapHpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPFreq, t } });
    grid->Add(1 + t, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapHpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPRes, t } });
    grid->Add(1 + t, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpRes, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapXOver = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapXOver, t } });
    grid->Add(1 + t, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapXOver, Slider::SliderStyle::RotaryVerticalDrag));

    for (int i = 0; i < 9; i++)
      grid->MarkSection({ { 1 + t, 1 + i }, { 1, 1 } });
  }

  for(int t = 0; t < FFEchoTapCount; t++)
    grid->MarkSection({ { 1 + t, 0 }, { 1, 1 } });

  return grid;
}

static Component*
MakeEchoSectionMain(
  FBPlugGUI* plugGUI, bool global,
  FBParamComboBox** gTargetBoxOut,
  FBParamToggleButton** vOnToggleOut)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 0, 0 });
  auto vOnOrGTarget = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VOnOrGTarget, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, vOnOrGTarget));
  if (global)
  {
    auto gTargetBox = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, vOnOrGTarget);
    grid->Add(0, 1, gTargetBox);
    *gTargetBoxOut = gTargetBox;
  }
  else
  {
    auto vOnToggle = plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, vOnOrGTarget);
    grid->Add(0, 1, vOnToggle);
    *vOnToggleOut = vOnToggle;
  }
  auto vOrderOrGOrder = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VOrderOrGOrder, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, vOrderOrGOrder));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, vOrderOrGOrder));
  auto delaySmoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::DelaySmoothTime, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delaySmoothTime, Slider::SliderStyle::LinearHorizontal));
  auto delaySmoothBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::DelaySmoothBars, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothBars));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delaySmoothBars));
  auto gain = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Gain, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
  auto sync = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Sync, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  grid->MarkSection({ { 0, 0 }, { 2, 5 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEchoSectionTaps(
  FBPlugGUI* plugGUI, bool global, 
  FBMultiContentComponent* inidividualTapsGUI,
  FBParamToggleButton** tapsOnToggleOut,
  FBAutoSizeButton** showTapsEditorOut)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapsOn, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  auto tapsOnToggle = plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on);
  grid->Add(0, 1, tapsOnToggle);
  *tapsOnToggleOut = tapsOnToggle;
  auto mix = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapsMix, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));

  auto guiTapSelect = topo->gui.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoGUIParam::TapSelect, 0 } });
  auto tapSelectSlider = plugGUI->StoreComponent<FBGUIParamSlider>(plugGUI, guiTapSelect, Slider::SliderStyle::RotaryVerticalDrag);
  grid->Add(0, 2, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, guiTapSelect));
  grid->Add(0, 3, tapSelectSlider);
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoGUIParam::TapSelect, 0 } };
  tapSelectSlider->onValueChange = [plugGUI, inidividualTapsGUI, indices]() {
    inidividualTapsGUI->SelectContentIndex(plugGUI->HostContext()->GetGUIParamDiscrete(indices) - 1); };

  auto tapsEditor = MakeEchoTapsEditor(plugGUI, global);
  auto showTapsEditor = plugGUI->StoreComponent<FBAutoSizeButton>("Edit All");
  showTapsEditor->onClick = [plugGUI, tapsEditor]() { dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent(tapsEditor, 360, 250); };
  grid->Add(1, 2, 1, 2, showTapsEditor);
  *showTapsEditorOut = showTapsEditor;

  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEchoSectionTap(FBPlugGUI* plugGUI, bool global, int tap)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto tapOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapOn, tap } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapOn));
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapOn));
  auto tapDelayTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayTime, tap } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayTime));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapDelayTime, Slider::SliderStyle::LinearHorizontal));
  auto tapDelayBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayBars, tap } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayBars));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tapDelayBars));
  auto tapLevel = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLevel, tap } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLevel));
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLevel, Slider::SliderStyle::LinearHorizontal));
  auto tapBalance = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapBalance, tap } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapBalance));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapBalance, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapXOver = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapXOver, tap } });
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapXOver));
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapXOver, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapLpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPFreq, tap } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpFreq));
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapLpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPRes, tap } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpRes));
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapHpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPFreq, tap } });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpFreq));
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapHpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPRes, tap } });
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpRes));
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 9 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEchoSectionFeedback(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackOn, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto delayTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackDelayTime, 0 } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayTime));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delayTime, Slider::SliderStyle::LinearHorizontal));
  auto delayBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackDelayBars, 0 } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayBars));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delayBars));
  auto mix = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackMix, 0 } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::LinearHorizontal));
  auto amount = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackAmount, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amount));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));
  auto xOver = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackXOver, 0 } });
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, xOver));
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, xOver, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPFreq, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPRes, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPFreq, 0 } });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPRes, 0 } });
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 9 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEchoSectionReverb(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbOn, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto mix = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbMix, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  auto size = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbSize, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, size));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, size, Slider::SliderStyle::RotaryVerticalDrag));
  auto damp = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbDamp, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, damp));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, damp, Slider::SliderStyle::RotaryVerticalDrag));
  auto xOver = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbXOver, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, xOver));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, xOver, Slider::SliderStyle::RotaryVerticalDrag));
  auto apf = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbAPF, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, apf));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, apf, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPFreq, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPRes, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPFreq, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPRes, 0 } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 10 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGEchoTab(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();

  FBParamComboBox* gEchoTargetBox = {};
  FBParamToggleButton* tapsOnToggle = {};
  FBParamToggleButton* vEchoOnToggle = {};
  FBAutoSizeButton* showTapsEditorButton = {};
  std::vector<int> columnSizes = { 1, 0, 0, 0, 0 };

  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto individualTapsGUI = plugGUI->StoreComponent<FBMultiContentComponent>();
  for(int i = 0; i < FFEchoTapCount; i++)
    individualTapsGUI->SetContent(i, MakeEchoSectionTap(plugGUI, global, i));
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoGUIParam::TapSelect, 0 } };
  individualTapsGUI->SelectContentIndex(plugGUI->HostContext()->GetGUIParamDiscrete(indices) - 1);

  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeEchoSectionMain(plugGUI, global, &gEchoTargetBox, &vEchoOnToggle));
  grid->Add(0, 1, MakeEchoSectionTaps(plugGUI, global, individualTapsGUI, &tapsOnToggle, &showTapsEditorButton));
  grid->Add(0, 2, individualTapsGUI);
  grid->Add(0, 3, MakeEchoSectionFeedback(plugGUI, global));
  grid->Add(0, 4, MakeEchoSectionReverb(plugGUI, global));

  FBParamTopoIndices tapsOnIndices = { { (int)moduleType, 0 }, { (int)FFEchoParam::TapsOn, 0 } };
  FBParamTopoIndices onOrTargetIndices = { { (int)moduleType, 0 }, { (int)FFEchoParam::VOnOrGTarget, 0 } };
  std::function<void()> updateTapEditEnabled = [plugGUI, global, showTapsEditorButton, onOrTargetIndices, tapsOnIndices]() {
    bool echoOn;
    bool tapsOn = plugGUI->HostContext()->GetAudioParamBool(tapsOnIndices);
    if (global)
      echoOn = plugGUI->HostContext()->GetAudioParamList<int>(onOrTargetIndices) != 0;
    else
      echoOn = plugGUI->HostContext()->GetAudioParamBool(onOrTargetIndices);
    showTapsEditorButton->setEnabled(echoOn && tapsOn); };
  updateTapEditEnabled();

  tapsOnToggle->onStateChange = updateTapEditEnabled;
  if(global)
    gEchoTargetBox->onChange = updateTapEditEnabled;
  else
    vEchoOnToggle->onStateChange = updateTapEditEnabled;

  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeEchoGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::EchoSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  tabs->AddModuleTab(false, true, { (int)FFModuleType::VEcho, 0 }, MakeGEchoTab(plugGUI, false));
  tabs->AddModuleTab(false, true, { (int)FFModuleType::GEcho, 0 }, MakeGEchoTab(plugGUI, true));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}