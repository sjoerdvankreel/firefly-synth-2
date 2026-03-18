#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
#include <firefly_synth/modules/effect/FFEffectGUI.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

FFEffectParamListener::
~FFEffectParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFEffectParamListener::
FFEffectParamListener(FBPlugGUI* plugGUI):
_plugGUI(plugGUI)
{
  _plugGUI->AddParamListener(this);
}

void 
FFEffectParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool /*changedFromUI*/)
{
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  if (indices.module.index != (int)FFModuleType::VEffect && indices.module.index != (int)FFModuleType::GEffect)
    return;
  if (indices.param.index == (int)FFEffectParam::EnvAmt || indices.param.index == (int)FFEffectParam::LFOAmt)
  {
    // TODO
    _plugGUI->RepaintSlidersForAudioParam({ { indices.module.index, indices.module.slot }, { (int)FFEffectParam::DistDrive, indices.param.slot } });
    _plugGUI->RepaintSlidersForAudioParam({ { indices.module.index, indices.module.slot }, { (int)FFEffectParam::StVarFreqFreq, indices.param.slot } });
    _plugGUI->RepaintSlidersForAudioParam({ { indices.module.index, indices.module.slot }, { (int)FFEffectParam::StVarPitchCoarse, indices.param.slot } });
    _plugGUI->RepaintSlidersForAudioParam({ { indices.module.index, indices.module.slot }, { (int)FFEffectParam::CombFreqFreqMin, indices.param.slot } });
    _plugGUI->RepaintSlidersForAudioParam({ { indices.module.index, indices.module.slot }, { (int)FFEffectParam::CombPitchCoarseMin, indices.param.slot } });
    _plugGUI->RepaintSlidersForAudioParam({ { indices.module.index, indices.module.slot }, { (int)FFEffectParam::CombFreqFreqPlus, indices.param.slot } });
    _plugGUI->RepaintSlidersForAudioParam({ { indices.module.index, indices.module.slot }, { (int)FFEffectParam::CombPitchCoarsePlus, indices.param.slot } });
  }
}

bool
FFEffectAdjustParamModulationGUIBounds(
  FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm)
{
  auto const& rtParam = ctx->Topo()->audio.params[index];
  int staticIndex = ctx->Topo()->modules[rtParam.runtimeModuleIndex].topoIndices.index;
  if (staticIndex != (int)FFModuleType::VEffect && staticIndex != (int)FFModuleType::GEffect)
    return false;

  // TODO
  if (rtParam.topoIndices.param.index == (int)FFEffectParam::DistDrive ||
    rtParam.topoIndices.param.index == (int)FFEffectParam::StVarFreqFreq ||
    rtParam.topoIndices.param.index == (int)FFEffectParam::StVarPitchCoarse ||
    rtParam.topoIndices.param.index == (int)FFEffectParam::CombFreqFreqMin ||
    rtParam.topoIndices.param.index == (int)FFEffectParam::CombPitchCoarseMin ||
    rtParam.topoIndices.param.index == (int)FFEffectParam::CombFreqFreqPlus ||
    rtParam.topoIndices.param.index == (int)FFEffectParam::CombPitchCoarsePlus)
  {
    double modAmountEnv = ctx->GetAudioParamNormalized({ { staticIndex, rtParam.topoIndices.module.slot }, { (int)FFEffectParam::EnvAmt, rtParam.topoIndices.param.slot } });
    FFApplyGUIModulationBounds(FFModulationOpType::UPStack, 0.0f, 1.0f, (float)modAmountEnv, currentMinNorm, currentMaxNorm);
    double modAmountLFO = ctx->GetAudioParamNormalized({ { staticIndex, rtParam.topoIndices.module.slot }, { (int)FFEffectParam::LFOAmt, rtParam.topoIndices.param.slot } });
    FFApplyGUIModulationBounds(FFModulationOpType::BPStack, 0.0f, 1.0f, (float)modAmountLFO, currentMinNorm, currentMaxNorm);
    return true;
  }

  return false;
}

static std::string
MakeEffectDetailLabel(
  FBRuntimeTopo const* topo, bool global, 
  std::vector<double> const& normalized)
{
  std::vector<FFEffectKind> kinds = {};
  auto moduleType = global ? FFModuleType::GEffect : FFModuleType::VEffect;
  auto const& moduleTopo = topo->static_->modules[(int)moduleType];
  for (int i = 0; i < FFEffectBlockCount; i++)
    kinds.push_back(moduleTopo.NormalizedToListFast<FFEffectKind>(FFEffectParam::Kind, (float)normalized[i]));

  bool haveAny = false;
  std::string result = {};
  for (int i = 0; i < FFEffectBlockCount; i++)
    if (kinds[i] != FFEffectKind::Off)
    {
      if (haveAny)
        result += ", ";
      result += FFEffectKindToString(kinds[i]);
      haveAny = true;
    }
  return result;
}

static Component*
MakeEffectSectionMain(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::On, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "On", FBLabelAlign::Left, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto oversample = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Oversample, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  grid->MarkSection({ { 0, 0 }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeEffectSectionBlock(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot, int block)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto style = block == 0 ? Slider::SliderStyle::LinearHorizontal : Slider::SliderStyle::RotaryVerticalDrag;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, 0, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, block == 0? 1: 0 });
  auto kind = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, block } });
  grid->Add(0, 0, 2, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, std::string(1, static_cast<char>('A' + block)), FBLabelAlign::Center, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, kind));
  auto filterMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FilterMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, filterMode));
  auto clipMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::ClipMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, clipMode));
  auto foldMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FoldMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, foldMode));
  auto skewMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::SkewMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewMode));
  auto compMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CompMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, compMode));

  auto distDrive = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistDrive, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distDrive));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distDrive, style));
  auto distMix = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistMix, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distMix));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distMix, style));

  auto stVarGain = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarGain, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarGain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarGain, style));
  auto stVarFreqFreq = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarFreqFreq, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarFreqFreq));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarFreqFreq, style));
  auto stVarPitchCoarse = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarPitchCoarse, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarPitchCoarse));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarPitchCoarse, style));
  auto combFreqFreqPlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqFreqPlus, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqFreqPlus));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqFreqPlus, style));
  auto combPitchCoarsePlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombPitchCoarsePlus, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combPitchCoarsePlus));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combPitchCoarsePlus, style));
  auto combFreqFreqMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqFreqMin, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqFreqMin));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqFreqMin, style));
  auto combPitchCoarseMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombPitchCoarseMin, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combPitchCoarseMin));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combPitchCoarseMin, style));

  auto compThreshold = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CompThreshold, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, compThreshold));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, compThreshold, style));

  grid->MarkSection({ { 0, 0 }, { 2, 1 } }, FBGridSectionMark::DefaultBackground);
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeEffectTab(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  std::vector<int> columnSizes = {};
  columnSizes.push_back(0);
  columnSizes.push_back(1);
  for (int i = 0; i < FFEffectBlockCount - 1; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeEffectSectionMain(plugGUI, moduleType, moduleSlot));
  for(int i = 0; i < FFEffectBlockCount; i++)
    grid->Add(0, 1 + i, MakeEffectSectionBlock(plugGUI, moduleType, moduleSlot, i));
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)moduleType, moduleSlot, grid);
}

static Component*
MakeEffectDetail(FBPlugGUI* plugGUI, bool global, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEffect : FFModuleType::VEffect;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0, 0 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 });

  auto lastKeySmoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::LastKeySmoothTime, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lastKeySmoothTime));
  grid->Add(0, 3, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lastKeySmoothTime, Slider::SliderStyle::LinearHorizontal));
  auto trackingKey = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::TrackingKey, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, trackingKey));
  grid->Add(0, 7, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, trackingKey, Slider::SliderStyle::LinearHorizontal));

  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    auto kind = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, i } });
    grid->Add(1 + i, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, std::string(1, static_cast<char>('A' + i))));
    grid->Add(1 + i, 1, plugGUI->StoreComponent<FBParamDisplayLabel>(plugGUI, kind));
    auto lfoAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::LFOAmt, i } });
    grid->Add(1 + i, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfoAmt));
    grid->Add(1 + i, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfoAmt, Slider::SliderStyle::RotaryVerticalDrag));
    auto envAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::EnvAmt, i } });
    grid->Add(1 + i, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, envAmt));
    grid->Add(1 + i, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, envAmt, Slider::SliderStyle::RotaryVerticalDrag));
    auto stVarKeyTrak = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarKeyTrak, i } });
    grid->Add(1 + i, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarKeyTrak));
    grid->Add(1 + i, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarKeyTrak, Slider::SliderStyle::RotaryVerticalDrag));
    auto combKeyTrk = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombKeyTrk, i } });
    grid->Add(1 + i, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combKeyTrk));
    grid->Add(1 + i, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combKeyTrk, Slider::SliderStyle::RotaryVerticalDrag));
    auto distAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistAmt, i } });
    grid->Add(1 + i, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distAmt));
    grid->Add(1 + i, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distAmt, Slider::SliderStyle::RotaryVerticalDrag));
    auto stVarRes = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarRes, i } });
    grid->Add(1 + i, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarRes));
    grid->Add(1 + i, 9, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarRes, Slider::SliderStyle::LinearHorizontal));
    auto distBias = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistBias, i } });
    grid->Add(1 + i, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distBias));
    grid->Add(1 + i, 9, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distBias, Slider::SliderStyle::LinearHorizontal));
    auto combResPlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombResPlus, i } });
    grid->Add(1 + i, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResPlus));
    grid->Add(1 + i, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResPlus, Slider::SliderStyle::RotaryVerticalDrag, true, false));
    auto combResMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombResMin, i } });
    grid->Add(1 + i, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResMin));
    grid->Add(1 + i, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResMin, Slider::SliderStyle::RotaryVerticalDrag));
  }

  for (int i = 0; i < 5; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 12 } }, FBGridSectionMark::AlternateBackground);

  return plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, false, true, false, grid);
}

Component*
FFMakeEffectGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::FXSelectedTab, 0 } });
  auto select = plugGUI->StoreComponent<FBSelectComponent>(plugGUI, moduleParam, std::vector<int> { 1, 1 }, std::vector<int> { 1, 0, 0, 0, 0 });
  select->AddLabel(0, 0, "VFX");
  for (int i = 0; i < FFEffectCount; i++)
    select->AddSelector(0, i + 1, { (int)FFModuleType::VEffect, i }, std::to_string(i + 1), MakeEffectTab(plugGUI, FFModuleType::VEffect, i));
  select->AddLabel(1, 0, "GFX");
  for (int i = 0; i < FFEffectCount; i++)
    select->AddSelector(1, i + 1, { (int)FFModuleType::GEffect, i }, std::to_string(i + 1), MakeEffectTab(plugGUI, FFModuleType::GEffect, i));
  select->ActivateStoredSelection();
  return plugGUI->StoreComponent<FBThemedComponent>(plugGUI, (int)FFThemedComponentId::EffectSelector, select);
}

Component*
FFMakeEffectDetailGUI(FBPlugGUI* plugGUI, bool global, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEffect : FFModuleType::VEffect;
  std::vector<FBRuntimeParam const*> params = {};
  int index = topo->moduleTopoToRuntime.at({ (int)moduleType, moduleSlot });
  auto name = topo->modules[index].name;
  for (int i = 0; i < FFEffectBlockCount; i++)
    params.push_back(topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, i } }));
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 1 }, std::vector<int> { 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, FBAsciiToUpper(name), FBLabelAlign::Right, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBMultiParamDisplayLabel>(plugGUI, params,
    [topo, global](std::vector<double> const& normalized) { return MakeEffectDetailLabel(topo, global, normalized); }));
  grid->Add(1, 0, 1, 2, MakeEffectDetail(plugGUI, global, moduleSlot));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } }, FBGridSectionMark::DefaultBackground);
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, false, true, card);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)moduleType, 0, margin);
}