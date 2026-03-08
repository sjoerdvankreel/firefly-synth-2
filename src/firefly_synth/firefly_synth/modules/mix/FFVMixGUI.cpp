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
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
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
MakeVMixSectionOsciAndVFXToOut(FBPlugGUI* plugGUI)
{
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  columnSizes.push_back(0);
  assert(FFOsciCount == FFEffectCount);
  for (int i = 0; i < FFOsciCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Osc\U00002192Out"));
  for (int o = 0; o < FFOsciCount; o++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToOut, o } });
    grid->Add(0, 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "VFX\U00002192Out"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::VFXToOut, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Osc\U00002192Osc Mix"));
  for (int o = 0; o < FFOsciCount; o++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToOsciMix, o } });
    grid->Add(0, 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Osc Mix\U00002192VFX"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciMixToVFX, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeVMixGUISectionAmpBalAndOsciMixToOut(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo(); 
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 1, 0, 0 });
  auto ampEnvTargetGrid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  auto ampEnvTarget = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::AmpEnvTarget, 0 } });
  ampEnvTargetGrid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, ampEnvTarget, true, FBLabelColors::PrimaryBackground));
  ampEnvTargetGrid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, ampEnvTarget));
  grid->Add(0, 0, ampEnvTargetGrid);
  auto oscMixToOutGrid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  auto oscMixToOut = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciMixToOut, 0 } });
  oscMixToOutGrid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Osc Mix\U00002192Out"));
  oscMixToOutGrid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, oscMixToOut, Slider::SliderStyle::RotaryVerticalDrag));
  grid->Add(1, 0, oscMixToOutGrid);
  auto amp = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0}, { (int)FFVMixParam::Amp, 0} });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amp));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amp, Slider::SliderStyle::LinearHorizontal));
  auto ampEnvToAmp = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::AmpEnvToAmp, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, ampEnvToAmp));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ampEnvToAmp, Slider::SliderStyle::RotaryVerticalDrag));
  auto bal = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::Bal, 0 } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bal));
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bal, Slider::SliderStyle::LinearHorizontal));
  auto lfo6ToBal = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::LFO6ToBal, 0 } });
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo6ToBal));
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo6ToBal, Slider::SliderStyle::RotaryVerticalDrag));
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeVMixDetail(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { 0, 0, 0, 0, 0, 0, 0 }, std::vector<int> { 1, 1, 1, 1, 1 });
  for (int o = 0; o < FFOsciCount; o++)
  {
    std::string name = "Osc " + std::to_string(o + 1) + "\U00002192VFX";
    grid->Add(o, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, name));
    for (int e = 0; e < FFEffectCount; e++)
    {
      int route = o * FFEffectCount + e;
      auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToVFX, route } });
      grid->Add(o, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::LinearHorizontal));
    }
  }
  for (int s = 0; s < FFMixFXToFXCount; s++)
  {
    int row = FFOsciCount + s / 2;
    int col = s % 2;
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0}, { (int)FFVMixParam::VFXToVFX, s } });
    grid->Add(row, col * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
    grid->Add(row, col * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::LinearHorizontal));
  }
  for (int i = 0; i < FFOsciCount + 3; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 5 } }, FBGridSectionMark::AlternateBackground);
  return grid;
}

Component*
FFMakeVMixGUITab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 1, 0, 0 });
  grid->Add(0, 0, MakeVMixGUISectionAmpBalAndOsciMixToOut(plugGUI));
  grid->Add(0, 1, MakeVMixSectionOsciToOsciMixAndOsciMixToVFX(plugGUI));
  grid->Add(0, 2, MakeVMixSectionOsciAndVFXToOut(plugGUI));
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::VMix, 0, grid);
}

Component*
FFMakeVMixDetailGUI(FBPlugGUI* plugGUI)
{ 
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  int index = topo->moduleTopoToRuntime.at({ (int)FFModuleType::VMix, 0 });
  auto name = topo->modules[index].name;
  auto target = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::AmpEnvTarget, 0 } });
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 1 }, std::vector<int> { 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, FBAsciiToUpper(name), FBLabelAlign::Right, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamDisplayLabel>(plugGUI, target));
  grid->Add(1, 0, 1, 2, MakeVMixDetail(plugGUI));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } }, FBGridSectionMark::DefaultBackground);
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, false, true, card);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::VMix, 0, margin);
}
