#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBImageComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>

using namespace juce;
  
Component*     
FFMakeHeaderGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();  
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { { 1, 1 } }, std::vector<int> { { 0, 0, 0, 0 } });
  bool isFX = plugGUI->HostContext()->Topo()->static_->meta.isFx;
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, isFX? FFPlugNameFX: FFPlugNameInst));
  auto format = plugGUI->HostContext()->Topo()->static_->meta.format;
  std::string formatName = format == FBPlugFormat::VST3 ? "VST3" : "CLAP";
#if FB_APPLE_AARCH64      
  std::string archName = "ARM";
#else
  std::string archName = "X64";
#endif
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, formatName + " " + archName + " " + FF_PLUG_VERSION));
  grid->Add(0, 1, 2, 1, plugGUI->StoreComponent<FBImageComponent>(plugGUI, 56, "header.png", RectanglePlacement::Flags::centred));
  grid->Add(0, 2, plugGUI->StoreComponent<FBFillerComponent>(2, 1));
  grid->MarkSection({ { 0, 0 }, { 2, 3 } }, FBGridSectionMark::BackgroundAndBorder, 5.0f, 2);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, true, true, grid);
  return plugGUI->StoreComponent<FBThemedComponent>(plugGUI->HostContext()->Topo(), (int)FFThemedComponentId::Header, margin);
};