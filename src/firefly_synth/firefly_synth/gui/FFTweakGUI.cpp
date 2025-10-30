#include <firefly_synth/gui/FFTweakGUI.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

Component*
FFMakeTweakGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 1, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Last Tweaked"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBLastTweakedLabel>(plugGUI));
  grid->Add(0, 2, plugGUI->StoreComponent<FBLastTweakedTextBox>(plugGUI, 100));
  grid->MarkSection({ { 0, 0 }, { 1, 3 } });
  auto subSection = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(subSection);
}