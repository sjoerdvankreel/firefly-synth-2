#include <firefly_synth/gui/FFTweakGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>

using namespace juce;

Component*
FFMakeTweakGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();      
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 0, 1, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Tweak"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBLastTweakedLabel>(plugGUI, 80));
  auto box = plugGUI->StoreComponent<FBLastTweakedTextBox>(plugGUI, 60);
  grid->Add(0, 2, plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, true, true, box));
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, true, false, false, card);
  return plugGUI->StoreComponent<FBThemedComponent>(plugGUI, (int)FFThemedComponentId::LastTweaked, margin);
}