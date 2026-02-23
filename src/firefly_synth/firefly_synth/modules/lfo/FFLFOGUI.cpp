#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
#include <firefly_synth/modules/lfo/FFLFOGUI.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
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

static std::string
MakeLFODetailLabel(
  FBRuntimeTopo const* topo, bool global, 
  std::vector<double> const& normalized)
{
  std::vector<FFLFOWaveMode> modes = {};
  std::vector<FFModulationOpType> opTypes = {};
  auto moduleType = global ? FFModuleType::GLFO : FFModuleType::VLFO;
  auto const& moduleTopo = topo->static_->modules[(int)moduleType];
  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    opTypes.push_back(moduleTopo.NormalizedToListFast<FFModulationOpType>(FFLFOParam::OpType, (float)normalized[2 * i]));
    modes.push_back(moduleTopo.NormalizedToListFast<FFLFOWaveMode>(FFLFOParam::WaveMode, (float)normalized[2 * i + 1]));
  }

  bool haveAny = false;
  std::string result = {};
  for (int i = 0; i < FFLFOBlockCount; i++)
    if (opTypes[i] != FFModulationOpType::Off)
    {
      if (haveAny)
        result += ", ";
      result += FFLFOWaveModeToString(modes[i]);
      haveAny = true;
    }
  return result;
}

static Component*
MakeLFOSectionMain(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type, true, FBLabelColors::PrimaryBackground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type, "One Shot"));
  auto smoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SmoothTime, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SmoothBars, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  auto seed = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Seed, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));
  auto sync = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Sync, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } }, FBGridSectionMark::DefaultBackgroundAlternateBorder);
  return grid;
}

static Component*
MakeLFOSectionBlock(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot, int block)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0 });
  auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, block } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLinkedLabel>(plugGUI, opType, std::string(1, static_cast<char>('A' + block)), FBLabelColors::PrimaryBackground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));
  auto waveMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::WaveMode, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, waveMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, waveMode));
  auto rateHz = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateHz, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rateHz));
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, rateHz, Slider::SliderStyle::RotaryVerticalDrag));
  auto rateBars = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateBars, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rateBars));
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, rateBars));  
  grid->MarkSection({ { 0, 0 }, { 2, 3 } }, block % 2 == 0? FBGridSectionMark::DefaultBackgroundDefaultBorder : FBGridSectionMark::DefaultBackgroundAlternateBorder);
  return grid;
}

static Component*
MakeLFOTab(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  std::vector<int> columnSizes = {};
  columnSizes.push_back(0);
  for (int i = 0; i < FFLFOBlockCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeLFOSectionMain(plugGUI, moduleType, moduleSlot));
  for (int i = 0; i < FFLFOBlockCount; i++)
    grid->Add(0, 1 + i, MakeLFOSectionBlock(plugGUI, moduleType, moduleSlot, i));
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, true, true, grid);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)moduleType, moduleSlot, margin);
}

static Component*
MakeLFODetail(FBPlugGUI* plugGUI, bool global, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GLFO : FFModuleType::VLFO;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0, 0, 0, 0 }, std::vector<int> { 0, 0, 1, 0, 0, 0, 1, 0, 0 });

  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, std::string(1, static_cast<char>('A'))));
  auto skewAXMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAXMode, 0 } });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewAXMode));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewAXMode));
  auto skewAXAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAXAmt, 0 } });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewAXAmt));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skewAXAmt, Slider::SliderStyle::RotaryVerticalDrag));
  auto skewAYMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAYMode, 0 } });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewAYMode));
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewAYMode));
  auto skewAYAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAYAmt, 0 } });
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewAYAmt));
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skewAYAmt, Slider::SliderStyle::RotaryVerticalDrag));

  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    grid->Add(1 + i * 2, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, std::string(1, static_cast<char>('A' + i))));
    auto min = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Min, i } });
    grid->Add(1 + i * 2, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, min));
    grid->Add(1 + i * 2, 2, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, min, Slider::SliderStyle::LinearHorizontal));
    auto max = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Max, i } });
    grid->Add(1 + i * 2, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, max));
    grid->Add(1 + i * 2, 6, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, max, Slider::SliderStyle::LinearHorizontal));
    grid->Add(1 + i * 2 + 1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, std::string(1, static_cast<char>('A' + i))));
    auto phase = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Phase, i } });
    grid->Add(1 + i * 2 + 1, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, phase));
    grid->Add(1 + i * 2 + 1, 2, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, phase, Slider::SliderStyle::LinearHorizontal));
    auto steps = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Steps, i } });
    grid->Add(1 + i * 2 + 1, 5, plugGUI->StoreComponent<FBParamLabel>(plugGUI, steps));
    grid->Add(1 + i * 2 + 1, 6, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, steps, Slider::SliderStyle::LinearHorizontal));
  }

  for (int i = 0; i < 7; i += 2)
    grid->MarkSection({ { i, 0 }, { 1, 9 } }, FBGridSectionMark::AlternateBackground);

  return plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, false, true, false, grid);
}

Component*
FFMakeLFOGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::LFOSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  tabs->SetTabSeparatorText(0, "VLFO");
  for (int i = 0; i < FFLFOCount; i++)
    tabs->AddModuleTab(true, false, { (int)FFModuleType::VLFO, i }, MakeLFOTab(plugGUI, FFModuleType::VLFO, i));
  tabs->SetTabSeparatorText(FFLFOCount, "GLFO");
  for (int i = 0; i < FFLFOCount; i++)
    tabs->AddModuleTab(true, false, { (int)FFModuleType::GLFO, i }, MakeLFOTab(plugGUI, FFModuleType::GLFO, i));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}

Component*
FFMakeLFODetailGUI(FBPlugGUI* plugGUI, bool global, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleType = global ? FFModuleType::GLFO : FFModuleType::VLFO;
  std::vector<FBRuntimeParam const*> params = {};
  int index = topo->moduleTopoToRuntime.at({ (int)moduleType, moduleSlot });
  auto name = topo->modules[index].name;
  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    params.push_back(topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, i } }));
    params.push_back(topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::WaveMode, i } }));
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 1 }, std::vector<int> { 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, FBAsciiToUpper(name), FBLabelAlign::Right, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBMultiParamDisplayLabel>(plugGUI, params,
    [topo, global](std::vector<double> const& normalized) { return MakeLFODetailLabel(topo, global, normalized); }));
  grid->Add(1, 0, 1, 2, MakeLFODetail(plugGUI, global, moduleSlot));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } }, FBGridSectionMark::DefaultBackground);
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, false, true, card);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)moduleType, 0, margin);
}