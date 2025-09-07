#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBImageComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

Component*
FFMakeHeaderGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { { 1, 1 } }, std::vector<int> { { 0, 0 } });
  grid->Add(0, 0, 2, 1, plugGUI->StoreComponent<FBImageComponent>(68, "header.png", RectanglePlacement::Flags::centred));
  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(FF_PLUG_VERSION));
  auto format = plugGUI->HostContext()->Topo()->static_->meta.format;
  std::string formatName = format == FBPlugFormat::VST3 ? "VST3" : "CLAP";
#if FB_APPLE_AARCH64
  std::string archName = "ARM";
#else
  std::string archName = "X64";
#endif
  grid->Add(1, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(formatName + " " + archName));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
};