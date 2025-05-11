#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBLabel.hpp>
#include <playground_base/gui/controls/FBSlider.hpp>
#include <playground_base/gui/controls/FBToggleButton.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  auto on = topo->audio.ParamAtTopo({(int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::On, 0});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  grid->MarkSection({ 0, 0, 1, 2 });
  return grid;
}

static Component*
MakeSectionParams(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1});
  auto rate = topo->audio.ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rate));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, rate, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 1, 2 });
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
FFMakeGLFOGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::GLFO, TabFactory);
}