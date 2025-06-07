#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterGUI.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({(int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto mode = topo->audio.ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Mode, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
  grid->MarkSection({ 0, 0, 1, 4 });
  return grid;
}

static Component*
MakeSectionParams(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1, 0, 1, 0, 1 });
  auto freq = topo->audio.ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, freq));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, freq, Slider::SliderStyle::LinearHorizontal));
  auto res = topo->audio.ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, res));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, res, Slider::SliderStyle::LinearHorizontal));
  auto gain = topo->audio.ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 1, 6 });
  return grid;
}
  
static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionParams(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeGFilterGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::GFilter, TabFactory);
}