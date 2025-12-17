#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
#include <firefly_synth/modules/mix/FFMixGUI.hpp>
#include <firefly_synth/modules/mix/FFVMixGUI.hpp>
#include <firefly_synth/modules/mix/FFVMixTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FFVMixParamListener::
~FFVMixParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFVMixParamListener::
FFVMixParamListener(FBPlugGUI* plugGUI):
_plugGUI(plugGUI)
{
  _plugGUI->AddParamListener(this);
}

void 
FFVMixParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool /*changedFromUI*/)
{
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  if (indices.module.index != (int)FFModuleType::VMix)
    return;
  if (indices.param.index == (int)FFVMixParam::AmpEnvToAmp)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::Amp, 0 } });
  if (indices.param.index == (int)FFVMixParam::LFO6ToBal)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::Bal, 0 } });
}

bool
FFVMixAdjustParamModulationGUIBounds(
  FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm)
{
  auto const& rtParam = ctx->Topo()->audio.params[index];
  int staticIndex = ctx->Topo()->modules[rtParam.runtimeModuleIndex].topoIndices.index;
  if (staticIndex != (int)FFModuleType::VMix)
    return false;

  if (rtParam.topoIndices.param.index == (int)FFVMixParam::Amp)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::AmpEnvToAmp, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::UPMul, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  if (rtParam.topoIndices.param.index == (int)FFVMixParam::Bal)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::LFO6ToBal, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::BPStack, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  return false;
}

static Component*
MakeVMixSectionOsciToVFX(FBPlugGUI* plugGUI)
{
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  for (int i = 0; i < FFOsciCount / 2; i++)
  {
    columnSizes.push_back(0);
    for (int j = 0; j < FFEffectCount; j++)
      columnSizes.push_back(0);
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  for (int o = 0; o < FFOsciCount; o++)
  {
    int row = o / 2;
    int colStart = (o % 2) * (FFEffectCount + 1);
    std::string name = "Osc " + std::to_string(o + 1) + "\U00002192VFX";
    grid->Add(row, colStart, plugGUI->StoreComponent<FBAutoSizeLabel>(name));
    for (int e = 0; e < FFEffectCount; e++)
    {
      int route = o * FFEffectCount + e;
      auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToVFX, route } });
      grid->Add(row, colStart + 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
    }
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFVMixOsciToVFXCount / 2 + 2 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(false, grid);
}

static Component*
MakeVMixSectionOsciAndVFXToOut(FBPlugGUI* plugGUI)
{
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  columnSizes.push_back(0);
  assert(FFOsciCount == FFEffectCount);
  for (int i = 0; i < FFOsciCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Osc\U00002192Out"));
  for (int o = 0; o < FFOsciCount; o++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToOut, o } });
    grid->Add(0, 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("VFX\U00002192Out"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::VFXToOut, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFOsciCount + 1 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(false, grid);
}

static Component*
MakeVMixSectionOsciToOsciMixAndOsciMixToVFX(FBPlugGUI* plugGUI)
{
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  columnSizes.push_back(0);
  assert(FFOsciCount == FFEffectCount);
  for (int i = 0; i < FFOsciCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Osc\U00002192Osc Mix"));
  for (int o = 0; o < FFOsciCount; o++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToOsciMix, o } });
    grid->Add(0, 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Osc Mix\U00002192VFX"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciMixToVFX, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFOsciCount + 1 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(false, grid);
}

static Component*
MakeVMixGUISectionAmpBalAndOsciMixToOut(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo(); 
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 1 });
  auto amp = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0}, { (int)FFVMixParam::Amp, 0} });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amp));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amp, Slider::SliderStyle::RotaryVerticalDrag));
  auto ampEnvToAmp = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::AmpEnvToAmp, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, ampEnvToAmp));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ampEnvToAmp, Slider::SliderStyle::RotaryVerticalDrag));
  auto bal = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::Bal, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bal));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bal, Slider::SliderStyle::RotaryVerticalDrag));
  auto lfo6ToBal = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::LFO6ToBal, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo6ToBal));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo6ToBal, Slider::SliderStyle::RotaryVerticalDrag));
  auto ampEnvTargetGrid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  auto ampEnvTarget = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::AmpEnvTarget, 0 } });
  ampEnvTargetGrid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, ampEnvTarget));
  ampEnvTargetGrid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, ampEnvTarget));
  grid->Add(0, 4, ampEnvTargetGrid);
  auto oscMixToOutGrid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  auto oscMixToOut = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciMixToOut, 0 } });
  oscMixToOutGrid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Osc Mix\U00002192Out"));
  oscMixToOutGrid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, oscMixToOut, Slider::SliderStyle::LinearHorizontal));
  grid->Add(1, 4, oscMixToOutGrid);
  grid->MarkSection({ { 0, 0 }, { 2, 5 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(true, grid);
}

Component*
FFMakeVMixGUITab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0, 1 });
  grid->Add(0, 0, MakeVMixSectionOsciToVFX(plugGUI));
  grid->Add(0, 1, FFMakeMixGUISectionFXToFX(plugGUI, (int)FFModuleType::VMix, (int)FFVMixParam::VFXToVFX));
  grid->Add(0, 2, MakeVMixSectionOsciToOsciMixAndOsciMixToVFX(plugGUI));
  grid->Add(0, 3, MakeVMixSectionOsciAndVFXToOut(plugGUI));
  grid->Add(0, 4, MakeVMixGUISectionAmpBalAndOsciMixToOut(plugGUI));
  auto section = plugGUI->StoreComponent<FBSectionComponent>(grid);
  return plugGUI->StoreComponent<FBModuleComponent>((int)FFModuleType::VMix, 0, section);
}