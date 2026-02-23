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
#include <firefly_base/gui/components/FBSectionComponent.hpp>
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
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on, true, FBLabelColors::PrimaryBackground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto oversample = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Oversample, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } }, FBGridSectionMark::DefaultBackgroundAlternateBorder);
  return grid;
}

static Component*
MakeEffectSectionBlock(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot, int block)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto kind = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, block } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLinkedLabel>(plugGUI, kind, std::string(1, static_cast<char>('A' + block)), FBLabelColors::PrimaryBackground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, kind));
  auto filterMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FilterMode, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, filterMode));
  auto stVarMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, stVarMode));
  auto clipMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::ClipMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, clipMode));
  auto foldMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FoldMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, foldMode));
  auto skewMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::SkewMode, block } });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewMode));

  auto distMix = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistMix, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distMix));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distMix, Slider::SliderStyle::RotaryVerticalDrag));
  auto distDrive = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistDrive, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distDrive));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distDrive, Slider::SliderStyle::RotaryVerticalDrag));

  auto stVarFreqFreq = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarFreqFreq, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarFreqFreq));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarFreqFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto stVarPitchCoarse = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarPitchCoarse, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarPitchCoarse));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarPitchCoarse, Slider::SliderStyle::RotaryVerticalDrag));
  auto stVarRes = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarRes, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarRes));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarRes, Slider::SliderStyle::RotaryVerticalDrag));

  auto combFreqFreqPlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqFreqPlus, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqFreqPlus));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqFreqPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combPitchCoarsePlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombPitchCoarsePlus, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combPitchCoarsePlus));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combPitchCoarsePlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combResPlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombResPlus, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResPlus));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combFreqFreqMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqFreqMin, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqFreqMin));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqFreqMin, Slider::SliderStyle::RotaryVerticalDrag));
  auto combPitchCoarseMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombPitchCoarseMin, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combPitchCoarseMin));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combPitchCoarseMin, Slider::SliderStyle::RotaryVerticalDrag));
  auto combResMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombResMin, block } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResMin));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResMin, Slider::SliderStyle::RotaryVerticalDrag));

  grid->MarkSection({ { 0, 0 }, { 2, 6 } }, block % 2 != 0? FBGridSectionMark::DefaultBackgroundAlternateBorder : FBGridSectionMark::DefaultBackgroundDefaultBorder);
  return grid;
}

static Component*
MakeEffectTab(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  std::vector<int> columnSizes = {};
  columnSizes.push_back(0);
  for (int i = 0; i < FFEffectBlockCount; i++)
    columnSizes.push_back(1);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeEffectSectionMain(plugGUI, moduleType, moduleSlot));
  for(int i = 0; i < FFEffectBlockCount; i++)
    grid->Add(0, 1 + i, MakeEffectSectionBlock(plugGUI, moduleType, moduleSlot, i));
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, true, true, grid);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)moduleType, moduleSlot, margin);
}

static Component*
MakeEffectDetail(FBPlugGUI* plugGUI, bool global, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GEffect : FFModuleType::VEffect;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0, 0 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 });

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
    auto stVarGain = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarGain, i } });
    grid->Add(1 + i, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarGain));
    grid->Add(1 + i, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarGain, Slider::SliderStyle::LinearHorizontal));
    auto distBias = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistBias, i } });
    grid->Add(1 + i, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distBias));
    grid->Add(1 + i, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distBias, Slider::SliderStyle::LinearHorizontal));
  }

  for (int i = 0; i < 5; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 10 } }, FBGridSectionMark::AlternateBackground);

  return plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, false, true, false, grid);
}

Component*
FFMakeEffectGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::FXSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  tabs->SetTabSeparatorText(0, "VFX");
  for (int i = 0; i < FFEffectCount; i++)
    tabs->AddModuleTab(true, false, { (int)FFModuleType::VEffect, i }, MakeEffectTab(plugGUI, FFModuleType::VEffect, i));
  tabs->SetTabSeparatorText(FFEffectCount, "GFX");
  for (int i = 0; i < FFEffectCount; i++)
    tabs->AddModuleTab(true, false, { (int)FFModuleType::GEffect, i }, MakeEffectTab(plugGUI, FFModuleType::GEffect, i));
  tabs->ActivateStoredSelectedTab();
  return tabs;
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