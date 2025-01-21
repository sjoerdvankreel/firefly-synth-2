#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeGLFOSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 1});
  auto on = plugGUI->Topo()->ParamAtTopo({(int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::On, 0});
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(on));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamToggleButton>(plugGUI, on));
  auto rate = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(rate));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamSlider>(plugGUI, rate, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}

Component*
FFMakeGLFOGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::GLFO, MakeGLFOSectionAll);
}