#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
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
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

FFOsciParamListener::
~FFOsciParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFOsciParamListener::
FFOsciParamListener(FBPlugGUI* plugGUI):
_plugGUI(plugGUI)
{
  _plugGUI->AddParamListener(this);
}

void 
FFOsciParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool /*changedFromUI*/)
{
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  if (indices.module.index != (int)FFModuleType::Osci)
    return;
  if (indices.param.index == (int)FFOsciParam::EnvToGain)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::Osci, indices.module.slot }, { (int)FFOsciParam::Gain, 0 } });
  if (indices.param.index == (int)FFOsciParam::LFOToFine)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::Osci, indices.module.slot }, { (int)FFOsciParam::Fine, 0 } });
}

bool
FFOsciAdjustParamModulationGUIBounds(
  FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm)
{
  auto const& rtParam = ctx->Topo()->audio.params[index];
  int staticIndex = ctx->Topo()->modules[rtParam.runtimeModuleIndex].topoIndices.index;
  if (staticIndex != (int)FFModuleType::Osci)
    return false;

  if (rtParam.topoIndices.param.index == (int)FFOsciParam::Gain)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::Osci, rtParam.topoIndices.module.slot }, { (int)FFOsciParam::EnvToGain, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::UPMul, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  if (rtParam.topoIndices.param.index == (int)FFOsciParam::Fine)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::Osci, rtParam.topoIndices.module.slot }, { (int)FFOsciParam::LFOToFine, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::BPStack, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  return false;
}

static Component*
MakeOsciSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> autoSizeColForRow = { -1, -1 };
  std::vector<int> autoSizeRowForCol = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, autoSizeRowForCol, autoSizeColForRow, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type, true, FBLabelColors::PrimaryBackground));
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
  auto phase = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Phase, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, phase));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, phase, Slider::SliderStyle::RotaryVerticalDrag));
  auto keyTrack = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::KeyTrack, 0 } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, keyTrack));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, keyTrack));
  grid->MarkSection({ { 0, 0 }, { 2, 10 } }, FBGridSectionMark::DefaultBackgroundAlternateBorder);
  return grid;
}

static Component*
MakeOsciSectionUni(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto count = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::UniCount, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, count, true, FBLabelColors::PrimaryBackground));
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
  grid->MarkSection({ { 0, 0 }, { 2, 6 } }, FBGridSectionMark::DefaultBackgroundDefaultBorder);
  return grid;
}

static Component*
MakeOsciSectionWave(FBPlugGUI* plugGUI, int moduleSlot)
{
  int rowCount = FFOsciWaveBasicCount + FFOsciWavePWCount + 2;
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int>(rowCount, 0), std::vector<int> { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 });
  for (int i = 0; i < FFOsciWaveBasicCount; i++)
  {
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveBasicMode, i } });
    grid->Add(i, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(i, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveBasicGain, i } });
    grid->Add(i, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(i, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
  }
  for (int i = 0; i < FFOsciWavePWCount; i++)
  {
    int offset = FFOsciWaveBasicCount;
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WavePWMode, i } });
    grid->Add(i + offset, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(i + offset, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WavePWGain, i } });
    grid->Add(i + offset, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(i + offset, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
    auto pw = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WavePWPW, i } });
    grid->Add(i + offset, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, pw));
    grid->Add(i + offset, 5, 1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, pw, Slider::SliderStyle::LinearHorizontal));
  }
  {
    int offset = FFOsciWaveBasicCount + FFOsciWavePWCount;
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveHSMode, 0 } });
    grid->Add(offset, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(offset, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveHSGain, 0 } });
    grid->Add(offset, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(offset, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
    auto pitch = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveHSPitch, 0 } });
    grid->Add(offset, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, pitch));
    grid->Add(offset, 5, 1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, pitch, Slider::SliderStyle::LinearHorizontal));
  }
  {
    int offset = FFOsciWaveBasicCount + FFOsciWavePWCount + 1;
    auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFMode, 0  } });
    grid->Add(offset, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
    grid->Add(offset, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
    auto gain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFGain, 0 } });
    grid->Add(offset, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
    grid->Add(offset, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
    auto over = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFOver, 0 } });
    grid->Add(offset, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, over));
    grid->Add(offset, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, over, Slider::SliderStyle::RotaryVerticalDrag));
    auto bw = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFBW, 0 } });
    grid->Add(offset, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bw));
    grid->Add(offset, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bw, Slider::SliderStyle::RotaryVerticalDrag));
    auto distance = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFDistance, 0 } });
    grid->Add(offset, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distance));
    grid->Add(offset, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distance, Slider::SliderStyle::RotaryVerticalDrag));
    auto decay = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::WaveDSFDecay, 0 } });
    grid->Add(offset, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decay));
    grid->Add(offset, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decay, Slider::SliderStyle::RotaryVerticalDrag));
  }
  for (int i = 0; i < rowCount; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 10 } }, FBGridSectionMark::AlternateBackground);

  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Off || vs[0] == (int)FFOsciType::Wave; });
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, grid, 0, indices, dependencies);
}

static Component*
MakeOsciSectionFM(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0, 0 }, std::vector<int> { 0, 1, 1, 1 });
  
  auto fmMode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMMode, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fmMode, true));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmMode));
  auto fmRatioMode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioMode, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fmRatioMode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmRatioMode));

  auto fmRatioRatio0 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioRatio, 0 } });
  for (int i = 0; i < 2; i++)
  {
    grid->Add(1, i * 2, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, fmRatioRatio0->static_.slotFormatter(*topo->static_, moduleSlot, i)));
    auto fmRatioRatio = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioRatio, i } });
    grid->Add(1, i * 2 + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmRatioRatio));
    auto fmRatioFree = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMRatioFree, i } });
    grid->Add(1, i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmRatioFree, Slider::SliderStyle::LinearHorizontal));
  }

  grid->Add(2, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Index 1\U000021921/2/3"));
  auto fmIndex11 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 0 } });
  grid->Add(2, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex11, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex12 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 1 } });
  grid->Add(2, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex12, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex13 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 2 } });
  grid->Add(2, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex13, Slider::SliderStyle::LinearHorizontal));

  grid->Add(3, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Index 2\U000021921/2/3"));
  auto fmIndex21 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 3 } });
  grid->Add(3, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex21, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex22 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 4 } });
  grid->Add(3, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex22, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex23 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 5 } });
  grid->Add(3, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex23, Slider::SliderStyle::LinearHorizontal));

  grid->Add(4, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Index 3\U000021921/2/3"));
  auto fmIndex31 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 6 } });
  grid->Add(4, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex31, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex32 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 7 } });
  grid->Add(4, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex32, Slider::SliderStyle::LinearHorizontal));
  auto fmIndex33 = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::FMIndex, 8 } });
  grid->Add(4, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex33, Slider::SliderStyle::LinearHorizontal));

  for (int i = 0; i < 5; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 4 } }, FBGridSectionMark::AlternateBackground);

  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, grid, 0, indices, dependencies);
}

static Component*
MakeOsciSectionString(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 1, 0, 0, 0, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0, 0, 0 }, columnSizes);

  auto excite = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringExcite, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, excite));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, excite, Slider::SliderStyle::LinearHorizontal));
  auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringMode, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
  auto seed = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringSeed, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));
  auto trackingKey = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringTrackingKey, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, trackingKey));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, trackingKey, Slider::SliderStyle::RotaryVerticalDrag));

  auto color = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringColor, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, color));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, color, Slider::SliderStyle::LinearHorizontal));
  auto x = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringX, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, x));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, x, Slider::SliderStyle::RotaryVerticalDrag));
  auto y = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringY, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, y));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, y, Slider::SliderStyle::RotaryVerticalDrag));
  auto poles = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringPoles, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, poles));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, poles, Slider::SliderStyle::RotaryVerticalDrag));

  auto lpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPFreq, 0 } });
  grid->Add(2, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(2, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::LinearHorizontal));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPRes, 0 } });
  grid->Add(2, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(2, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPOn, 0 } });
  grid->Add(2, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpOn));
  grid->Add(2, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, lpOn));
  auto lpKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringLPKTrk, 0 } });
  grid->Add(2, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpKTrk));
  grid->Add(2, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpKTrk, Slider::SliderStyle::RotaryVerticalDrag));

  auto hpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPFreq, 0 } });
  grid->Add(3, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(3, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::LinearHorizontal));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPRes, 0 } });
  grid->Add(3, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(3, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPOn, 0 } });
  grid->Add(3, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpOn));
  grid->Add(3, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, hpOn));
  auto hpKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringHPKTrk, 0 } });
  grid->Add(3, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpKTrk));
  grid->Add(3, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpKTrk, Slider::SliderStyle::RotaryVerticalDrag));

  auto damp = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringDamp, 0 } });
  grid->Add(4, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, damp));
  grid->Add(4, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, damp, Slider::SliderStyle::LinearHorizontal));
  auto dampKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringDampKTrk, 0 } });
  grid->Add(4, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dampKTrk));
  grid->Add(4, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dampKTrk, Slider::SliderStyle::RotaryVerticalDrag));

  auto feedback = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringFeedback, 0 } });
  grid->Add(5, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedback));
  grid->Add(5, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedback, Slider::SliderStyle::LinearHorizontal));
  auto feedbackKTrk = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::StringFeedbackKTrk, 0 } });
  grid->Add(5, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackKTrk));
  grid->Add(5, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackKTrk, Slider::SliderStyle::RotaryVerticalDrag));

  for (int i = 0; i < 6; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 8 } }, FBGridSectionMark::AlternateBackground);

  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, grid, 0, indices, dependencies);
}

static Component*
MakeOsciSectionExtAudio(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 1, 0, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0 }, columnSizes);

  auto inputGain = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioInputGain, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, inputGain));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, inputGain, Slider::SliderStyle::LinearHorizontal));
  auto inputBal = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioInputBal, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, inputBal));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, inputBal, Slider::SliderStyle::LinearHorizontal));
  auto lpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioLPFreq, 0 } });
  grid->Add(2, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpFreq));
  grid->Add(2, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpFreq, Slider::SliderStyle::LinearHorizontal));
  auto lpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioLPRes, 0 } });
  grid->Add(2, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpRes));
  grid->Add(2, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto lpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioLPOn, 0 } });
  grid->Add(2, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lpOn));
  grid->Add(2, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, lpOn));
  auto hpFreq = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioHPFreq, 0 } });
  grid->Add(3, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpFreq));
  grid->Add(3, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpFreq, Slider::SliderStyle::LinearHorizontal));
  auto hpRes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioHPRes, 0 } });
  grid->Add(3, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpRes));
  grid->Add(3, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hpRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto hpOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::ExtAudioHPOn, 0 } });
  grid->Add(3, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hpOn));
  grid->Add(3, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, hpOn));

  for (int i = 0; i < 4; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 6 } }, FBGridSectionMark::AlternateBackground);

  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::ExtAudio; });
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, grid, 0, indices, dependencies);
}

static Component*
MakeOsciTab(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  grid->Add(0, 0, MakeOsciSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeOsciSectionUni(plugGUI, moduleSlot));
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, true, true, grid);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::Osci, moduleSlot, margin);
}

static Component*
MakeOsciDetail(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 1 });
  grid->Add(0, 0, MakeOsciSectionWave(plugGUI, moduleSlot));
  grid->Add(0, 0, MakeOsciSectionFM(plugGUI, moduleSlot));
  grid->Add(0, 0, MakeOsciSectionString(plugGUI, moduleSlot));
  grid->Add(0, 0, MakeOsciSectionExtAudio(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, false, true, false, grid);
}

Component*
FFMakeOsciGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::OscSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  tabs->SetTabSeparatorText(0, "Osc");
  for (int i = 0; i < FFOsciCount; i++)
    tabs->AddModuleTab(true, false, { (int)FFModuleType::Osci, i }, MakeOsciTab(plugGUI, i));
  tabs->AddModuleTab(true, false, { (int)FFModuleType::OsciMod, 0 }, FFMakeOsciModTab(plugGUI));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}

Component*
FFMakeOsciDetailGUI(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  int index = topo->moduleTopoToRuntime.at({ (int)FFModuleType::Osci, moduleSlot });
  auto name = topo->modules[index].name;
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Type, 0 } });
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 1 }, std::vector<int> { 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, FBAsciiToUpper(name), FBLabelAlign::Right, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamDisplayLabel>(plugGUI, type));
  grid->Add(1, 0, 1, 2, MakeOsciDetail(plugGUI, moduleSlot));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } }, FBGridSectionMark::DefaultBackground);
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, false, true, card);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::Osci, moduleSlot, margin);
}