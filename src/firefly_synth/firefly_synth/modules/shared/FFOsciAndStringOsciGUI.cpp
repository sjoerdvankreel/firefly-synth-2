#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciGUI.hpp>
#include <firefly_synth/modules/shared/FFOsciAndStringOsciGUI.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>

using namespace juce;

Component*
FFMakeOsciAndStringOsciGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI);
  for (int i = 0; i < FFOsciCount; i++)
    tabs->AddModuleTab({ (int)FFModuleType::Osci, i }, FFMakeOsciTab(plugGUI, i));
  for (int i = 0; i < FFStringOsciCount; i++)
    tabs->AddModuleTab({ (int)FFModuleType::StringOsci, i }, FFMakeStringOsciTab(plugGUI, i));
  return tabs;
}