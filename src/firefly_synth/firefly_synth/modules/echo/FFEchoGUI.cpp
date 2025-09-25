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
  auto columnSizes = std::vector<int> { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, rowSizes, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Tap/Param"));
  grid->MarkSection({ { 0, 0 }, { 1, 1 } });

  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>("On"));
  auto tapDelayLevel0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLevel, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(tapDelayLevel0->static_.display));
  auto tapDelayTime0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayTime, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>(tapDelayTime0->static_.display));
  auto tapDelayBars0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayBars, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>(tapDelayBars0->static_.display));
  auto tapBalance0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapBalance, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBAutoSizeLabel>(tapBalance0->static_.display));
  auto tapXOver0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapXOver, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBAutoSizeLabel>(tapXOver0->static_.display));
  auto tapLpOn0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPOn, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBAutoSizeLabel>(tapLpOn0->static_.display));
  auto tapLpFreq0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPFreq, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBAutoSizeLabel>(tapLpFreq0->static_.display));
  auto tapLpRes0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPRes, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBAutoSizeLabel>(tapLpRes0->static_.display));
  auto tapHpOn0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPOn, 0 } });
  grid->Add(0, 9, plugGUI->StoreComponent<FBAutoSizeLabel>(tapHpOn0->static_.display));
  auto tapHpFreq0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPFreq, 0 } });
  grid->Add(0, 10, plugGUI->StoreComponent<FBAutoSizeLabel>(tapHpFreq0->static_.display));
  auto tapHpRes0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPRes, 0 } });
  grid->Add(0, 11, plugGUI->StoreComponent<FBAutoSizeLabel>(tapHpRes0->static_.display));

  for(int i = 0; i < 11; i++)
    grid->MarkSection({ { 0, 1 + i }, { 1, 1 } });  

  for (int t = 0; t < FFEchoTapCount; t++)
  {
    grid->Add(t + 1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(t + 1), true));
    auto tapOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapOn, t } });
    grid->Add(1 + t, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapOn));
    auto tapLevel = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLevel, t } });
    grid->Add(1 + t, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLevel, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapDelayTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayTime, t } });
    grid->Add(1 + t, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapDelayTime, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapDelayBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayBars, t } });
    grid->Add(1 + t, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tapDelayBars));
    auto tapBalance = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapBalance, t } });
    grid->Add(1 + t, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapBalance, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapXOver = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapXOver, t } });
    grid->Add(1 + t, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapXOver, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapLpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPOn, t } });
    grid->Add(1 + t, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapLpOn));
    auto tapLpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPFreq, t } });
    grid->Add(1 + t, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapLpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPRes, t } });
    grid->Add(1 + t, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpRes, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapHpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPOn, t } });
    grid->Add(1 + t, 9, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapHpOn));
    auto tapHpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPFreq, t } });
    grid->Add(1 + t, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapHpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPRes, t } });
    grid->Add(1 + t, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpRes, Slider::SliderStyle::RotaryVerticalDrag));

    for (int i = 0; i < 11; i++)
      grid->MarkSection({ { 1 + t, 1 + i }, { 1, 1 } });
  }

  for(int t = 0; t < FFEchoTapCount; t++)
    grid->MarkSection({ { 1 + t, 0 }, { 1, 1 } });

  return grid;
}

static Component*
MakeEchoSectionMain(
  FBPlugGUI* plugGUI, bool global,
  FBParamComboBox** targetBoxOut)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 1, 0, 1, 0 });
  auto vTargetOrGTarget = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VTargetOrGTarget, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, vTargetOrGTarget));
  auto targetBox = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, vTargetOrGTarget);
  grid->Add(0, 1, targetBox);
  *targetBoxOut = targetBox;
  auto order = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, order));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, order));
  auto delaySmoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::DelaySmoothTime, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delaySmoothTime, Slider::SliderStyle::LinearHorizontal));
  auto delaySmoothBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::DelaySmoothBars, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothBars));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delaySmoothBars));
  auto gain = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Gain, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
  auto voiceExtendTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceExtendTime, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceExtendTime));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceExtendTime, Slider::SliderStyle::LinearHorizontal));
  auto voiceExtendBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceExtendBars, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceExtendBars));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, voiceExtendBars));
  auto voiceFadeTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceFadeTime, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceFadeTime));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceFadeTime, Slider::SliderStyle::LinearHorizontal));
  auto voiceFadeBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceFadeBars, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceFadeBars));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, voiceFadeBars));
  auto sync = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Sync, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  grid->MarkSection({ { 0, 0 }, { 2, 7 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEchoSectionTaps(
  FBPlugGUI* plugGUI, bool global,
  FBParamToggleButton** tapsOnToggle,
  FBAutoSizeButton** showTapsEditor)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 1, 0 });

  auto tapsEditor = MakeEchoTapsEditor(plugGUI, global);
  *showTapsEditor = plugGUI->StoreComponent<FBAutoSizeButton>("Multi Tap");
  (*showTapsEditor)->onClick = [plugGUI, tapsEditor, global]() { 
    std::string title = std::string(global ? "G" : "V") + "Echo Multi Tap";
    dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent(title, tapsEditor, 400, 250, [plugGUI, global]() {
      auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
      FBTopoIndices moduleIndices = { (int)moduleType, 0 };
      std::string name = plugGUI->HostContext()->Topo()->ModuleAtTopo(moduleIndices)->name;
      plugGUI->HostContext()->UndoState().Snapshot("Init " + name + " Taps");
      for (int p = (int)FFEchoParam::TapFirst; p <= (int)FFEchoParam::TapLast; p++)
        for(int s = 0; s < FFEchoTapCount; s++)
          plugGUI->HostContext()->DefaultAudioParam({ { moduleIndices }, { p, s } });
    });
  };
  grid->Add(0, 0, *showTapsEditor);

  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapsOn, 0 } });
  *tapsOnToggle = plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on);
  grid->Add(0, 1, *tapsOnToggle);

  auto mix = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapsMix, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::LinearHorizontal));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));

  grid->MarkSection({ { 0, 0 }, { 2, 2 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEchoSectionFeedback(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 });
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
  auto lpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPOn, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpOn));
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, lpOn));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPFreq, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPRes, 0 } });
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPOn, 0 } });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpOn));
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, hpOn));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPFreq, 0 } });
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPRes, 0 } });
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 11 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEchoSectionReverb(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
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
  auto lpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPOn, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpOn));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, lpOn));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPFreq, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPRes, 0 } });
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPOn, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpOn));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, hpOn));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPFreq, 0 } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPRes, 0 } });
  grid->Add(1, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 12 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGEchoTab(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();

  FBParamComboBox* echoTargetBox = {};
  FBParamToggleButton* tapsOnToggle = {};
  FBAutoSizeButton* showTapsEditorButton = {};
  std::vector<int> columnSizes = { 7, 0, 6, 0 };

  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeEchoSectionMain(plugGUI, global, &echoTargetBox));
  grid->Add(0, 1, MakeEchoSectionTaps(plugGUI, global, &tapsOnToggle, &showTapsEditorButton));
  grid->Add(0, 2, MakeEchoSectionFeedback(plugGUI, global));
  grid->Add(0, 3, MakeEchoSectionReverb(plugGUI, global));

  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices tapsOnIndices = { { (int)moduleType, 0 }, { (int)FFEchoParam::TapsOn, 0 } };
  FBParamTopoIndices vTargetOrGTargetIndices = { { (int)moduleType, 0 }, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  std::function<void()> updateTapEditEnabled = [plugGUI, showTapsEditorButton, vTargetOrGTargetIndices, tapsOnIndices]() {
    bool tapsOn = plugGUI->HostContext()->GetAudioParamBool(tapsOnIndices);
    bool echoOn = plugGUI->HostContext()->GetAudioParamList<int>(vTargetOrGTargetIndices) != 0;
    showTapsEditorButton->setEnabled(echoOn && tapsOn); };
  updateTapEditEnabled();

  tapsOnToggle->onStateChange = updateTapEditEnabled;
  echoTargetBox->onChange = updateTapEditEnabled;
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