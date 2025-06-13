#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciGUI.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciTopo.hpp>

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
MakeStringOsciSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Gain, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  auto coarse = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Coarse, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, coarse));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, coarse, Slider::SliderStyle::RotaryVerticalDrag));
  auto fine = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Fine, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fine));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fine, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 4 });
  return grid;
}

static Component*
MakeStringOsciSectionUni(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto count = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::UniCount, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, count));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, count, Slider::SliderStyle::RotaryVerticalDrag));
  auto detune = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::UniDetune, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, detune));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, detune, Slider::SliderStyle::RotaryVerticalDrag));
  auto spread = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::UniSpread, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, spread));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag));
  auto blend = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::UniBlend, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, blend));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, blend, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 4 });
  return grid;
}

static Component*
MakeStringOsciSectionNoise(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto seed = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Seed, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));  
  auto excite = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Excite, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, excite));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, excite, Slider::SliderStyle::RotaryVerticalDrag));  
  auto color = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Color, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, color));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, color, Slider::SliderStyle::RotaryVerticalDrag));
  auto poles = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Poles, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, poles));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, poles, Slider::SliderStyle::RotaryVerticalDrag));
  auto x = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::X, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, x));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, x, Slider::SliderStyle::RotaryVerticalDrag));
  auto y = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Y, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, y));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, y, Slider::SliderStyle::RotaryVerticalDrag));  
  grid->MarkSection({ 0, 0, 2, 6 });
  return grid;
}

static Component*
MakeStringOsciSectionKS(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 });
  auto lpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::LPOn, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpOn));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, lpOn));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, (int)FFStringOsciParam::LPFreq, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::LinearHorizontal));
  auto hpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::HPOn, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpOn));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, hpOn));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::HPFreq, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::LinearHorizontal));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::LPRes, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::HPRes, 0 } });
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::LPKTrk, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpKTrk));
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::HPKTrk, 0 } });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpKTrk));
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto damp = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Damp, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, damp));
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, damp, Slider::SliderStyle::LinearHorizontal));
  auto dampKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::DampKTrk, 0 } });
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dampKTrk));
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dampKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto feedback = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::Feedback, 0 } });
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedback));
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedback, Slider::SliderStyle::LinearHorizontal));
  auto feedbackKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::FeedbackKTrk, 0 } });
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackKTrk));
  grid->Add(1, 10, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto trackingKey = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::TrackingKey, 0 } });
  grid->Add(0, 11, plugGUI->StoreComponent<FBParamLabel>(plugGUI, trackingKey));
  grid->Add(0, 12, plugGUI->StoreComponent<FBParamSlider>(plugGUI, trackingKey, Slider::SliderStyle::RotaryVerticalDrag));
  auto trackingRange = topo->audio.ParamAtTopo({ { (int)FFModuleType::StringOsci, moduleSlot }, { (int)FFStringOsciParam::TrackingRange, 0 } });
  grid->Add(1, 11, plugGUI->StoreComponent<FBParamLabel>(plugGUI, trackingRange));
  grid->Add(1, 12, plugGUI->StoreComponent<FBParamSlider>(plugGUI, trackingRange, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 13 });
  return grid;
}

Component*
FFMakeStringOsciTab(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 1 });
  grid->Add(0, 0, MakeStringOsciSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeStringOsciSectionUni(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeStringOsciSectionNoise(plugGUI, moduleSlot));
  grid->Add(0, 3, MakeStringOsciSectionKS(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}