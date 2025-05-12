#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/output/FFOutputGUI.hpp>
#include <playground_plug/modules/output/FFOutputTopo.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBLabel.hpp>
#include <playground_base/gui/controls/FBSlider.hpp>
#include <playground_base/gui/controls/FBComboBox.hpp>
#include <playground_base/gui/controls/FBOutputLabel.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0 } );
  auto voices = topo->audio.ParamAtTopo({ (int)FFModuleType::Output, moduleSlot, (int)FFOutputParam::Voices, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voices));
  grid->Add(0, 1, plugGUI->StoreComponent<FBOutputParamLabel>(plugGUI, voices, "0", std::to_string(FBMaxVoices)));
  grid->MarkSection({ 0, 0, 1, 2 });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
} 

Component*
FFMakeOutputGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Output, MakeSectionAll);
}