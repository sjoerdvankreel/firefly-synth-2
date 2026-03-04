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
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static std::string
MakeEchoDetailLabel(FBRuntimeParam const* order, std::vector<double> const& normalized)
{
  std::vector<int> orders;
  bool tapsOn = normalized[1] > 0.5;
  bool feedbackOn = normalized[2] > 0.5;
  bool reverbOn = normalized[3] > 0.5;
  std::array<bool, 3> componentOn = { tapsOn, feedbackOn, reverbOn };
  std::vector<std::string> componentNames = { "Multi Tap", "Feedback", "Reverb" };
  auto orderVal = (FFEchoOrder)order->static_.ListNonRealTime().NormalizedToPlainFast((float)normalized[0]);
  switch (orderVal)
  {
  case FFEchoOrder::TapsToFeedbackToReverb: orders = { 0, 1, 2 }; break;
  case FFEchoOrder::TapsToReverbToFeedback: orders = { 0, 2, 1 }; break;
  case FFEchoOrder::FeedbackToReverbToTaps: orders = { 1, 2, 0 }; break;
  case FFEchoOrder::FeedbackToTapsToReverb: orders = { 1, 0, 2 }; break;
  case FFEchoOrder::ReverbToFeedbackToTaps: orders = { 2, 1, 0 }; break;
  case FFEchoOrder::ReverbToTapsToFeedback: orders = { 2, 0, 1 }; break;
  default: FB_ASSERT(false);
  }
  bool haveAny = false;
  std::string result = {};
  for (int i = 0; i < 3; i++)
    if (componentOn[orders[i]])
    {
      if (haveAny)
        result += ", ";
      result += componentNames[orders[i]];
      haveAny = true;
    }
  return result;
}

static Component* 
MakeEchoTapsEditor(FBPlugGUI* plugGUI, bool global)
{
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto topo = plugGUI->HostContext()->Topo();
  auto rowSizes = std::vector<int>();
  rowSizes.push_back(1);
  for (int i = 0; i < FFEchoTapCount; i++)
    rowSizes.push_back(1);
  auto columnSizes = std::vector<int> { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, rowSizes, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Tap/Param"));
  grid->MarkSection({ { 0, 0 }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);

  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "On"));
  grid->MarkSection({ { 0, 1 }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);

  auto tapDelayLevel0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLevel, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapDelayLevel0->static_.display));
  auto tapDelayTime0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayTime, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapDelayTime0->static_.display));
  auto tapDelayBars0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapDelayBars, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapDelayBars0->static_.display));
  auto tapBalance0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapBalance, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapBalance0->static_.display));
  auto tapXOver0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapXOver, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapXOver0->static_.display));
  grid->MarkSection({ { 0, 2 }, { 1, 4 } }, FBGridSectionMark::DefaultBackground);

  auto tapLpOn0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPOn, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapLpOn0->static_.display));
  auto tapLpFreq0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPFreq, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapLpFreq0->static_.display));
  auto tapLpRes0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPRes, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapLpRes0->static_.display));
  grid->MarkSection({ { 0, 6 }, { 1, 3 } }, FBGridSectionMark::DefaultBackground);

  auto tapHpOn0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPOn, 0 } });
  grid->Add(0, 9, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapHpOn0->static_.display));
  auto tapHpFreq0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPFreq, 0 } });
  grid->Add(0, 10, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapHpFreq0->static_.display));
  auto tapHpRes0 = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPRes, 0 } });
  grid->Add(0, 11, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, tapHpRes0->static_.display));
  grid->MarkSection({ { 0, 9 }, { 1, 3 } }, FBGridSectionMark::DefaultBackground);

  for (int t = 0; t < FFEchoTapCount; t++)
  {
    grid->Add(t + 1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, std::to_string(t + 1), FBLabelAlign::Center));
    grid->MarkSection({ { t + 1, 0 }, { 1, 1 } });

    auto tapOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapOn, t } });
    grid->Add(1 + t, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapOn));
    grid->MarkSection({ { t + 1, 1 }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);

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
    grid->MarkSection({ { t + 1, 2 }, { 1, 4 } }, FBGridSectionMark::DefaultBackground);

    auto tapLpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPOn, t } });
    grid->Add(1 + t, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapLpOn));
    auto tapLpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPFreq, t } });
    grid->Add(1 + t, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapLpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapLPRes, t } });
    grid->Add(1 + t, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpRes, Slider::SliderStyle::RotaryVerticalDrag));
    grid->MarkSection({ { t + 1, 6 }, { 1, 3 } }, FBGridSectionMark::DefaultBackground);

    auto tapHpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPOn, t } });
    grid->Add(1 + t, 9, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapHpOn));
    auto tapHpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPFreq, t } });
    grid->Add(1 + t, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
    auto tapHpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapHPRes, t } });
    grid->Add(1 + t, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpRes, Slider::SliderStyle::RotaryVerticalDrag));
    grid->MarkSection({ { t + 1, 9 }, { 1, 3 } }, FBGridSectionMark::DefaultBackground, { true, 3.0f, t == FFEchoTapCount - 1 ? 2 : 0 });
  }

  return grid;
}

static Component*
MakeEchoSectionMain(
  FBPlugGUI* plugGUI, bool global,
  FBParamComboBox** targetBoxOut)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto vTargetOrGTarget = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VTargetOrGTarget, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, vTargetOrGTarget, true, FBLabelColors::PrimaryBackground));
  auto targetBox = plugGUI->StoreComponent<FBParamComboBox>(plugGUI, vTargetOrGTarget, "Osc 4 PostMix");
  grid->Add(0, 1, targetBox);
  *targetBoxOut = targetBox;
  auto sync = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Sync, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto order = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, order));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, order));
  auto gain = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Gain, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeEchoSectionTaps(
  FBPlugGUI* plugGUI, bool global,
  FBParamToggleButton** tapsOnToggle,
  FBAutoSizeButton** showTapsEditor)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 1, 0 });

  auto tapsEditor = MakeEchoTapsEditor(plugGUI, global);
  *showTapsEditor = plugGUI->StoreComponent<FBAutoSizeButton>(plugGUI, "Multi Tap");
  (*showTapsEditor)->setTooltip("Show Taps Editor");
  (*showTapsEditor)->onClick = [plugGUI, tapsEditor, global]() { 
    auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
    std::string title = std::string(global ? "G" : "V") + "Echo Multi Tap";
    dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent(title, (int)moduleType, 0, tapsEditor, 400, 275, true, [plugGUI, global]() {
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

  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeEchoSectionFeedback(FBPlugGUI* plugGUI, bool global)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackOn, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto mix = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackMix, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  auto delayTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackDelayTime, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayTime));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delayTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto delayBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackDelayBars, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayBars));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delayBars));
  auto amount = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackAmount, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amount));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));  
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeEchoSectionReverb(FBPlugGUI* plugGUI, bool global)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbOn, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto mix = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbMix, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  auto size = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbSize, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, size));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, size, Slider::SliderStyle::RotaryVerticalDrag));
  auto damp = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbDamp, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, damp));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, damp, Slider::SliderStyle::RotaryVerticalDrag));
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeEchoTab(FBPlugGUI* plugGUI, bool global)
{
  FBParamComboBox* echoTargetBox = {};
  FBParamToggleButton* tapsOnToggle = {};
  FBAutoSizeButton* showTapsEditorButton = {};
  std::vector<int> columnSizes = { 0, 0, 0, 0 };

  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, columnSizes);
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
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)moduleType, 0, grid);
}

static Component*
MakeEchoDetail(FBPlugGUI* plugGUI, bool global)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0, 0, 0, 0 }, std::vector<int> { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 });
  
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Feedback"));
  auto feedbackModRate = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackModRate, 0 } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackModRate));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackModRate, Slider::SliderStyle::RotaryVerticalDrag));
  auto feedbackModAmt = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackModAmt, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackModAmt));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackModAmt, Slider::SliderStyle::RotaryVerticalDrag));
  auto feedbackLpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPOn, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackLpOn));
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, feedbackLpOn));
  auto feedbackLpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPFreq, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackLpFreq));
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto feedbackLpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackLPRes, 0 } });
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackLpRes));
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackLpRes, Slider::SliderStyle::RotaryVerticalDrag));

  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Feedback"));
  auto feedbackXOver = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackXOver, 0 } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackXOver));
  grid->Add(1, 2, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackXOver, Slider::SliderStyle::LinearHorizontal));
  auto feedbackHpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPOn, 0 } });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackHpOn));
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, feedbackHpOn));
  auto feedbackHpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPFreq, 0 } });
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackHpFreq));
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto feedbackHpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackHPRes, 0 } });
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackHpRes));
  grid->Add(1, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackHpRes, Slider::SliderStyle::RotaryVerticalDrag));

  grid->Add(2, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Reverb"));
  auto reverbApf = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbAPF, 0 } });
  grid->Add(2, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbApf));
  grid->Add(2, 2, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, reverbApf, Slider::SliderStyle::LinearHorizontal));
  auto reverbLpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPOn, 0 } });
  grid->Add(2, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbLpOn));
  grid->Add(2, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, reverbLpOn));
  auto reverbLpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPFreq, 0 } });
  grid->Add(2, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbLpFreq));
  grid->Add(2, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, reverbLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto reverbLpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbLPRes, 0 } });
  grid->Add(2, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbLpRes));
  grid->Add(2, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, reverbLpRes, Slider::SliderStyle::RotaryVerticalDrag));
  
  grid->Add(3, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Reverb"));
  auto reverbXOver = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbXOver, 0 } });
  grid->Add(3, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbXOver));
  grid->Add(3, 2, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, reverbXOver, Slider::SliderStyle::LinearHorizontal));
  auto reverbHpOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPOn, 0 } });
  grid->Add(3, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbHpOn));
  grid->Add(3, 6, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, reverbHpOn));
  auto reverbHpFreq = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPFreq, 0 } });
  grid->Add(3, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbHpFreq));
  grid->Add(3, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, reverbHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto reverbHpRes = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbHPRes, 0 } });
  grid->Add(3, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, reverbHpRes));
  grid->Add(3, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, reverbHpRes, Slider::SliderStyle::RotaryVerticalDrag));

  auto delaySmoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::DelaySmoothTime, 0 } });
  grid->Add(4, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothTime));
  grid->Add(4, 2, 1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delaySmoothTime, Slider::SliderStyle::LinearHorizontal));
  auto delaySmoothBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::DelaySmoothBars, 0 } });
  grid->Add(4, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothBars));
  grid->Add(4, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delaySmoothBars));
  auto voiceExtendTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceExtendTime, 0 } });
  grid->Add(5, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceExtendTime));
  grid->Add(5, 2, 1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceExtendTime, Slider::SliderStyle::LinearHorizontal));
  auto voiceExtendBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceExtendBars, 0 } });
  grid->Add(5, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceExtendBars));
  grid->Add(5, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, voiceExtendBars));
  auto voiceFadeTime = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceFadeTime, 0 } });
  grid->Add(6, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceFadeTime));
  grid->Add(6, 2, 1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceFadeTime, Slider::SliderStyle::LinearHorizontal));
  auto voiceFadeBars = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::VoiceFadeBars, 0 } });
  grid->Add(6, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceFadeBars));
  grid->Add(6, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, voiceFadeBars));

  for (int i = 0; i < (global? 5: 7); i += 2)
      grid->MarkSection({ { i, 0 }, { 1, 11 } }, FBGridSectionMark::AlternateBackground);

  return plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, false, true, false, grid);
}

Component*
FFMakeEchoGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::EchoSelectedTab, 0 } });
  auto select = plugGUI->StoreComponent<FBSelectComponent>(plugGUI, moduleParam, std::vector<int> { 1, 1 }, std::vector<int> { 1 });
  select->AddSelector(0, 0, { (int)FFModuleType::VEcho, 0 }, "VEcho", MakeEchoTab(plugGUI, false));
  select->AddSelector(1, 0, { (int)FFModuleType::GEcho, 0 }, "GEcho", MakeEchoTab(plugGUI, true));
  select->ActivateStoredSelection();
  return plugGUI->StoreComponent<FBThemedComponent>(plugGUI, (int)FFThemedComponentId::EchoSelector, select);
}

Component*
FFMakeEchoDetailGUI(FBPlugGUI* plugGUI, bool global)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  int index = topo->moduleTopoToRuntime.at({ (int)moduleType, 0 });
  auto name = topo->modules[index].name;
  auto order = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } });
  auto tapsOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::TapsOn, 0 } });
  auto reverbOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbOn, 0 } });
  auto feedbackOn = topo->audio.ParamAtTopo({ { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackOn, 0 } });
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 1 }, std::vector<int> { 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, FBAsciiToUpper(name), FBLabelAlign::Right, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBMultiParamDisplayLabel>(plugGUI,
    std::vector<FBRuntimeParam const*> { order, tapsOn, feedbackOn, reverbOn },
    [order](std::vector<double> const& normalized) { return MakeEchoDetailLabel(order, normalized); }));
  grid->Add(1, 0, 1, 2, MakeEchoDetail(plugGUI, global));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } }, FBGridSectionMark::DefaultBackground);
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, false, true, card);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)moduleType, 0, margin);
}