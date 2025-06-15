#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGUISettingsTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "UI";
  result->tabName = "UI";
  result->slotCount = 1;
  result->id = "{2407B76A-7FB3-4BD6-B6FD-B1F610AF8147}";
  result->guiParams.resize((int)FFGUISettingsGUIParam::Count);
  auto selectGuiModule = [](auto& state) { return &state.guiSettings; }; 

  auto& guiUserScale = result->guiParams[(int)FFGUISettingsGUIParam::UserScale];
  guiUserScale.defaultText = "1";
  guiUserScale.name = "User Scale";
  guiUserScale.slotCount = 1;
  guiUserScale.id = "{48F2A59F-789F-4076-AB4B-4CC19B3A929A}";
  guiUserScale.type = FBParamType::Linear;
  guiUserScale.Linear().min = 0.5f;
  guiUserScale.Linear().max = 16.0f;
  auto selectGuiUserScale = [](auto& module) { return &module.userScale; };
  guiUserScale.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiUserScale);

  auto& guiGraphMode = result->guiParams[(int)FFGUISettingsGUIParam::GraphMode];
  guiGraphMode.defaultText = "If Focus";
  guiGraphMode.name = "Graph Mode";
  guiGraphMode.display = "Graph";
  guiGraphMode.slotCount = 1;
  guiGraphMode.id = "{6C5F2DC2-C796-444C-8D43-077708580609}";
  guiGraphMode.type = FBParamType::List;
  guiGraphMode.List().items = {
    { "{D5C9FC77-0DE8-4077-9D07-073B44F5076F}", "Basic" },
    { "{8291740E-D7DC-4481-B430-9C73F3343E10}", "Always" },
    { "{A5369260-7E9F-4C23-8FED-0C42CAA9DD91}", "If Focus" } };
  auto selectGuiGraphMode = [](auto& module) { return &module.graphMode; };
  guiGraphMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiGraphMode);

  return result;
}