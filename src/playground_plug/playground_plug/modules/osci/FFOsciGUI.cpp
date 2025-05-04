#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>
#include <playground_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto gain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  auto coarse = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Coarse, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, coarse));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, coarse, Slider::SliderStyle::LinearHorizontal));
  auto fine = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Fine, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fine));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fine, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 1, 4 });
  grid->MarkSection({ 1, 0, 1, 4 });
  return grid;
}

static Component*
MakeSectionUnison(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto count = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UniCount, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, count));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, count, Slider::SliderStyle::RotaryVerticalDrag));
  auto offset = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UniOffset, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, offset));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, offset, Slider::SliderStyle::RotaryVerticalDrag));
  auto random = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UniRandom, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, random));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, random, Slider::SliderStyle::RotaryVerticalDrag));
  auto spread = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UniSpread, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, spread));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag));
  auto detune = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UniDetune, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, detune));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, detune, Slider::SliderStyle::RotaryVerticalDrag));
  auto blend = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UniBlend, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, blend));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, blend, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 6 });
  return grid;
}

static Component*
MakeSectionBasic(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 1, 0, 1, 0, 1, 0 });

  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Mode/PW"));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Gain/Sync"));
  for (int i = 0; i < FFOsciBasicCount; i++)
  {
    auto basicMode = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicMode, i });
    grid->Add(0, 1 + i * 2 + 0, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, basicMode));
    auto basicPW = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicPW, i });
    grid->Add(0, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicPW, Slider::SliderStyle::RotaryVerticalDrag));
    auto basicGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicGain, i });
    grid->Add(1, 1 + i * 2 + 0, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicGain, Slider::SliderStyle::LinearHorizontal));
    auto basicSync = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSync, i });
    grid->Add(1, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicSync, Slider::SliderStyle::RotaryVerticalDrag));
  }

  grid->MarkSection({ 0, 0, 2, 9 });

  // TODO helper function
  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Off || vs[0] == (int)FFOsciType::Basic; });
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, grid, 0, indices, dependencies);
}

static Component*
MakeSectionDSF(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 1});
  auto dsfMode = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFMode, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dsfMode));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, dsfMode));
  auto dsfOvertones = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFOvertones, 0 });
  grid->Add(1, 0, 1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dsfOvertones, Slider::SliderStyle::LinearHorizontal));
  auto dsfBandwidth = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFBandwidth, 0 });
  grid->Add(1, 0, 1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dsfBandwidth, Slider::SliderStyle::LinearHorizontal));
  auto dsfDistance = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFDistance, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dsfDistance));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dsfDistance, Slider::SliderStyle::LinearHorizontal));
  auto dsfDecay = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFDecay, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dsfDecay));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dsfDecay, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 2, 4 });

  // TODO helper function
  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF; });
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, grid, 0, indices, dependencies);
}

static Component*
MakeSectionFM(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 });
  
  auto fmExp = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMExp, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fmExp));
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, fmExp));

  auto fmRatioMode = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMRatioMode, 0 });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fmRatioMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmRatioMode));
  auto fmRatioRatio0 = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMRatioRatio, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(fmRatioRatio0->static_.slotFormatter(0)));
  grid->Add(1, 2, plugGUI->StoreComponent<FBAutoSizeLabel>(fmRatioRatio0->static_.slotFormatter(1)));
  for (int i = 0; i < 2; i++)
  {
    auto fmRatioRatio = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMRatioRatio, i });
    grid->Add(0 + i, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, fmRatioRatio));
    auto fmRatioFree = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMRatioFree, i });
    grid->Add(0 + i, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmRatioFree, Slider::SliderStyle::LinearHorizontal));
  }

  grid->Add(0, 4, plugGUI->StoreComponent<FBAutoSizeLabel>("1>1/2/3"));
  auto fmIndex11 = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMIndex, 0 });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex11, Slider::SliderStyle::RotaryVerticalDrag));
  auto fmIndex12 = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMIndex, 1 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex12, Slider::SliderStyle::RotaryVerticalDrag));
  auto fmIndex13 = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMIndex, 2 });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex13, Slider::SliderStyle::RotaryVerticalDrag));

  grid->Add(0, 6, plugGUI->StoreComponent<FBAutoSizeLabel>("2>1/2/3"));
  for (int i = 3; i < 6; i++)
  {
    auto fmIndex = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMIndex, i });
    grid->Add(0, 7 + i - 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex, Slider::SliderStyle::RotaryVerticalDrag));
  }

  grid->Add(1, 6, plugGUI->StoreComponent<FBAutoSizeLabel>("3>1/2/3"));
  for (int i = 6; i < 9; i++)
  {
    auto fmIndex = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::FMIndex, i });
    grid->Add(1, 7 + i - 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ 0, 0, 2, 10 });

  // TODO helper function
  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });
  return plugGUI->StoreComponent<FBParamsDependentComponent>(plugGUI, grid, 0, indices, dependencies);
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  // TODO move marksection to here
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionUnison(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeSectionBasic(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeSectionDSF(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeSectionFM(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeOsciGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Osci, TabFactory);
}