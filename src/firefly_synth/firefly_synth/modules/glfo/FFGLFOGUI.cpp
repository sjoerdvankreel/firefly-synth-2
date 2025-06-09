#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/glfo/FFGLFOGUI.hpp>
#include <firefly_synth/modules/glfo/FFGLFOTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeGLFOSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  auto on = topo->audio.ParamAtTopo({(int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::On, 0});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  grid->MarkSection({ 0, 0, 1, 2 });
  return grid;
}

static Component*
MakeGLFOSectionParams(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1});
  auto rate = topo->audio.ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rate));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, rate, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 1, 2 });
  return grid;
}

static Component*
GLFOTabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  grid->Add(0, 0, MakeGLFOSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeGLFOSectionParams(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeGLFOGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::GLFO, GLFOTabFactory);
}