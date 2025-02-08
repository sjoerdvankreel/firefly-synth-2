#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->ParamAtTopo({(int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}

static Component*
MakeSectionParams(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 1, 0, 1, 0, 1 });
  auto freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, freq));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, freq, Slider::SliderStyle::LinearHorizontal));
  auto res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, res));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, res, Slider::SliderStyle::LinearHorizontal));
  auto gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}
  
static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto result = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 1 });
  result->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  result->Add(0, 1, MakeSectionParams(plugGUI, moduleSlot));
  return result;
}

Component*
FFMakeGFilterGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::GFilter, TabFactory);
}