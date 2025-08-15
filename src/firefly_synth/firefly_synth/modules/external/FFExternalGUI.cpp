#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/external/FFExternalGUI.hpp>
#include <firefly_synth/modules/external/FFExternalTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeExternalSectionAll(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 1 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, columnSizes);
  auto hostSmoothTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::External, 0 }, { (int)FFExternalParam::HostSmoothTime, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hostSmoothTime));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hostSmoothTime, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
}

Component*
FFMakeExternalGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::External].name;
  tabs->addTab(name, {}, MakeExternalSectionAll(plugGUI), false);
  return tabs;
}