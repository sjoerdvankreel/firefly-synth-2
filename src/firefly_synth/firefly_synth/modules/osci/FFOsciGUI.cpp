#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

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
MakeOsciSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, 1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, 1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto coarse = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Coarse, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, coarse));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, coarse, Slider::SliderStyle::RotaryVerticalDrag));
  auto pan = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Pan, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, pan));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, pan, Slider::SliderStyle::RotaryVerticalDrag));
  auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Gain, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  auto envToGain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::EnvToGain, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, envToGain));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, envToGain, Slider::SliderStyle::RotaryVerticalDrag));
  auto fine = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Fine, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fine));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fine, Slider::SliderStyle::RotaryVerticalDrag));
  auto lfoToFine = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::LFOToFine, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfoToFine));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfoToFine, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 8 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeOsciSectionUni(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto count = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::UniCount, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, count));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, count, Slider::SliderStyle::RotaryVerticalDrag));
  auto detune = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::UniDetune, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, detune));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, detune, Slider::SliderStyle::RotaryVerticalDrag));
  auto spread = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::UniSpread, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, spread));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag));
  auto blend = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::UniBlend, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, blend));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, blend, Slider::SliderStyle::RotaryVerticalDrag));
  auto offset = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::UniOffset, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, offset));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, offset, Slider::SliderStyle::RotaryVerticalDrag));
  auto random = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::UniRandom, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, random));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, random, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeOsciSectionWave(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 });
  for (int i = 0; i < FFOsciWaveBasicCount; i++)
  {
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveBasicMode, i } });
    grid->Add(i, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(i, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveBasicGain, i } });
    grid->Add(i, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(i, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  }
  for (int i = 0; i < FFOsciWavePWCount; i++)
  {
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WavePWMode, i } });
    grid->Add(i, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(i, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WavePWGain, i } });
    grid->Add(i, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(i, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
    auto pw = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WavePWPW, i } });
    grid->Add(i, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, pw));
    grid->Add(i, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, pw, Slider::SliderStyle::RotaryVerticalDrag));
  }
  {
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveHSMode, 0 } });
    grid->Add(0, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(0, 11, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveHSGain, 0 } });
    grid->Add(0, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(0, 13, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
    auto pitch = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveHSPitch, 0 } });
    grid->Add(0, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, pitch));
    grid->Add(0, 15, 1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, pitch, Slider::SliderStyle::LinearHorizontal));
  }
  {
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFMode, 0  } });
    grid->Add(1, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(1, 11, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFGain, 0 } });
    grid->Add(1, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(1, 13, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
    auto over = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFOver, 0 } });
    grid->Add(1, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, over));
    grid->Add(1, 15, plugGUI->StoreComponent<FBParamSlider>(plugGUI, over, Slider::SliderStyle::RotaryVerticalDrag));
    auto bw = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFBW, 0 } });
    grid->Add(1, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bw));
    grid->Add(1, 15, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bw, Slider::SliderStyle::RotaryVerticalDrag));
    auto distance = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFDistance, 0 } });
    grid->Add(1, 16, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distance));
    grid->Add(1, 17, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distance, Slider::SliderStyle::RotaryVerticalDrag));
    auto decay = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFDecay, 0 } });
    grid->Add(1, 18, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decay));
    grid->Add(1, 19, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decay, Slider::SliderStyle::RotaryVerticalDrag));
  }

  grid->MarkSection({ { 0, 0 }, { 2, 20 } });

  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Off || vs[0] == (int)FFOsciType::Wave; });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, section, 0, indices, dependencies);
}

static Component*
MakeOsciSectionFM(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 });
  
  auto fmMode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMMode, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fmMode));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmMode));
  auto fmRatioMode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioMode, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fmRatioMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmRatioMode));

  auto fmRatioRatio0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioRatio, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(fmRatioRatio0->static_.slotFormatter(*topo->static_, moduleSlot, 0)));
  grid->Add(1, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(fmRatioRatio0->static_.slotFormatter(*topo->static_, moduleSlot, 1)));
  for (int i = 0; i < 2; i++)
  {
    auto fmRatioRatio = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioRatio, i } });
    grid->Add(0 + i, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmRatioRatio));
    auto fmRatioFree = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioFree, i } });
    grid->Add(0 + i, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmRatioFree, Slider::SliderStyle::RotaryVerticalDrag));
  }

  grid->Add(0, 4, plugGUI->StoreComponent<FBAutoSizeLabel>("1\U000021921/2/3"));
  auto fmIndex11 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex11, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex12 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 1 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex12, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex13 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 2 } });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex13, Slider::SliderStyle::LinearHorizontal));

  grid->Add(0, 6, plugGUI->StoreComponent<FBAutoSizeLabel>("2\U000021921/2/3"));
  auto fmIndex21 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 3 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex21, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex22 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 4 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex22, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex23 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 5 } });
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex23, Slider::SliderStyle::LinearHorizontal));

  grid->Add(0, 8, plugGUI->StoreComponent<FBAutoSizeLabel>("3\U000021921/2/3"));
  auto fmIndex31 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 6 } });
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex31, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex32 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 7 } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex32, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex33 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 8 } });
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex33, Slider::SliderStyle::LinearHorizontal));

  grid->MarkSection({ { 0, 0 }, { 2, 10 } });

  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, section, 0, indices, dependencies);
}

static Component*
MakeOsciSectionString(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);

  auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringMode, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
  auto excite = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringExcite, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, excite));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, excite, Slider::SliderStyle::LinearHorizontal));
  auto seed = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringSeed, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));
  auto trackingKey = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringTrackingKey, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, trackingKey));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, trackingKey, Slider::SliderStyle::RotaryVerticalDrag));
  auto color = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringColor, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, color));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, color, Slider::SliderStyle::RotaryVerticalDrag));
  auto poles = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringPoles, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, poles));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, poles, Slider::SliderStyle::RotaryVerticalDrag));
  auto x = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringX, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, x));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, x, Slider::SliderStyle::RotaryVerticalDrag));
  auto y = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringY, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, y));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, y, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 8 } });

  auto lpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPOn, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpOn));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, lpOn));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPFreq, 0 } });
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(0, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPOn, 0 } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpOn));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, hpOn));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPFreq, 0 } });
  grid->Add(1, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(1, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPRes, 0 } });
  grid->Add(0, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(0, 13, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPRes, 0 } });
  grid->Add(1, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(1, 13, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPKTrk, 0 } });
  grid->Add(0, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpKTrk));
  grid->Add(0, 15, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPKTrk, 0 } });
  grid->Add(1, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpKTrk));
  grid->Add(1, 15, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto damp = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringDamp, 0 } });
  grid->Add(0, 16, plugGUI->StoreComponent<FBParamLabel>(plugGUI, damp));
  grid->Add(0, 17, plugGUI->StoreComponent<FBParamSlider>(plugGUI, damp, Slider::SliderStyle::LinearHorizontal));
  auto dampKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringDampKTrk, 0 } });
  grid->Add(0, 18, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dampKTrk));
  grid->Add(0, 19, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dampKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto feedback = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringFeedback, 0 } });
  grid->Add(1, 16, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedback));
  grid->Add(1, 17, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedback, Slider::SliderStyle::LinearHorizontal));
  auto feedbackKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringFeedbackKTrk, 0 } });
  grid->Add(1, 18, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackKTrk));
  grid->Add(1, 19, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackKTrk, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 8 }, { 2, 12 } });

  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });
  auto section =  plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, section, 0, indices, dependencies);
}

static Component*
MakeOsciTab(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, MakeOsciSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeOsciSectionUni(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeOsciSectionWave(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeOsciSectionFM(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeOsciSectionString(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeOsciGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::OscSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  for (int i = 0; i < FFOsciCount; i++)
    tabs->AddModuleTab(i != 0, { (int)FFModuleType::Osci, i }, MakeOsciTab(plugGUI, i));
  tabs->AddModuleTab(true, { (int)FFModuleType::OsciMod, 0 }, FFMakeOsciModGUISectionAll(plugGUI));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}