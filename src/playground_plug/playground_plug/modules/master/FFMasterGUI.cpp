#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBOutputParamLabel.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 0, 1, 0, 1 } );
  auto voices = topo->audio.ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Voices, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voices));
  grid->Add(0, 1, plugGUI->StoreComponent<FBOutputParamLabel>(plugGUI, voices, "0", std::to_string(FBMaxVoices)));
  auto gain = topo->audio.ParamAtTopo({(int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Gain, 0});
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::LinearHorizontal));
  auto hostSmoothTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::HostSmoothTime, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hostSmoothTime));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hostSmoothTime, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
} 

Component*
FFMakeMasterGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Master, MakeSectionAll);
}