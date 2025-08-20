#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/echo/FFGEchoGUI.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>

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
MakeGEchoTapsEditor(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBAutoSizeLabel>("fubar");
}

static Component*
MakeGEchoSectionMain(FBPlugGUI* plugGUI, FBMultiContentComponent* tapsGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 0, 0, 0 });
  auto target = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::Target, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, target));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, target));
  auto order = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::Order, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, order));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, order));

  auto guiTapSelect = topo->gui.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoGUIParam::TapSelect, 0 } });
  auto tapSelectSlider = plugGUI->StoreComponent<FBGUIParamSlider>(plugGUI, guiTapSelect, Slider::SliderStyle::RotaryVerticalDrag);
  grid->Add(0, 2, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, guiTapSelect));
  grid->Add(0, 3, tapSelectSlider);
  FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoGUIParam::TapSelect, 0 } };
  tapSelectSlider->onValueChange = [plugGUI, tapsGUI, indices]() { tapsGUI->SelectContentIndex(plugGUI->HostContext()->GetGUIParamDiscrete(indices) - 1); };

  auto tapsMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapsMix, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapsMix));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapsMix, Slider::SliderStyle::RotaryVerticalDrag));
  auto sync = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::Sync, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  
  auto tapsEditor = MakeGEchoTapsEditor(plugGUI);
  auto showTapsEditor = plugGUI->StoreComponent<FBAutoSizeButton>("Taps");
  showTapsEditor->onClick = [plugGUI, tapsEditor]() { dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent(tapsEditor, 400, 250); };
  grid->Add(1, 4, 1, 2, showTapsEditor);

  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGEchoSectionTap(FBPlugGUI* plugGUI, int tap)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto tapOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapOn, tap } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapOn));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tapOn));
  auto tapLevel = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLevel, tap } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLevel));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLevel, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapDelayTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapDelayTime, tap } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapDelayTime, Slider::SliderStyle::LinearHorizontal));
  auto tapDelayBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapDelayBars, tap } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelayBars));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tapDelayBars));
  auto tapDelaySmoothTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapDelaySmoothTime, tap } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelaySmoothTime));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapDelaySmoothTime, Slider::SliderStyle::LinearHorizontal));
  auto tapDelaySmoothBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapDelaySmoothBars, tap } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapDelaySmoothBars));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tapDelaySmoothBars));
  auto tapBalance = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapBalance, tap } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapBalance));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapBalance, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapXOver = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapXOver, tap } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapXOver));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapXOver, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapLpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLPFreq, tap } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpFreq));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapLpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapLPRes, tap } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapLpRes));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapLpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapHpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapHPFreq, tap } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpFreq));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto tapHpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapHPRes, tap } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tapHpRes));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tapHpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 10 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGEchoSectionFeedback(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackType, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackMix, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  auto delayTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackDelayTime, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delayTime, Slider::SliderStyle::LinearHorizontal));
  auto delayBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackDelayBars, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayBars));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delayBars));
  auto delaySmoothTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackDelaySmoothTime, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothTime));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delaySmoothTime, Slider::SliderStyle::LinearHorizontal));
  auto delaySmoothBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackDelaySmoothBars, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delaySmoothBars));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delaySmoothBars));
  auto amount = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackAmount, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amount));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amount, Slider::SliderStyle::RotaryVerticalDrag));
  auto xOver = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackXOver, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, xOver));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, xOver, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackLPFreq, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackLPRes, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackHPFreq, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackHPRes, 0 } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 10 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGEchoSectionReverb(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbType, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbMix, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  auto size = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbSize, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, size));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, size, Slider::SliderStyle::RotaryVerticalDrag));
  auto damp = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbDamp, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, damp));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, damp, Slider::SliderStyle::RotaryVerticalDrag));
  auto xOver = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbXOver, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, xOver));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, xOver, Slider::SliderStyle::RotaryVerticalDrag));
  auto apf = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbAPF, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, apf));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, apf, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbLPFreq, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbLPRes, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbHPFreq, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::ReverbHPRes, 0 } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 10 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGEchoTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();

  auto taps = plugGUI->StoreComponent<FBMultiContentComponent>();
  for(int i = 0; i < FFGEchoTapCount; i++)
    taps->SetContent(i, MakeGEchoSectionTap(plugGUI, i));
  FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoGUIParam::TapSelect, 0 } };
  taps->SelectContentIndex(plugGUI->HostContext()->GetGUIParamDiscrete(indices) - 1);

  std::vector<int> columnSizes = { 1, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeGEchoSectionMain(plugGUI, taps));
  grid->Add(0, 1, taps);
  grid->Add(0, 2, MakeGEchoSectionFeedback(plugGUI));
  grid->Add(0, 3, MakeGEchoSectionReverb(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeGEchoGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::GEcho].name;
  tabs->addTab(name, {}, MakeGEchoTab(plugGUI), false);
  return tabs;
}