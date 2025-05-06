#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFGUIState.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGUISettingsTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "UI";
  result->slotCount = 1;
  result->id = "{2407B76A-7FB3-4BD6-B6FD-B1F610AF8147}";
  result->guiParams.resize((int)FFGUISettingsGUIParam::Count);
  auto selectGuiModule = [](auto& state) { return &state.guiSettings; }; 
  
  auto& guiGraphTrack = result->guiParams[(int)FFGUISettingsGUIParam::GraphTrack];
  guiGraphTrack.defaultText = "On";
  guiGraphTrack.display = "Graph Track";
  guiGraphTrack.name = "Graph Tracking";
  guiGraphTrack.slotCount = 1;
  guiGraphTrack.id = "{B45F4562-39D1-42CA-B600-0248C98CC292}";
  guiGraphTrack.type = FBParamType::Boolean;
  auto selectGuiGraphTrack = [](auto& module) { return &module.graphTrack; };
  guiGraphTrack.addrSelector = FFSelectGUIParamAddr(selectGuiModule, selectGuiGraphTrack);

  auto& guiUserScale = result->guiParams[(int)FFGUISettingsGUIParam::UserScale];
  guiUserScale.defaultText = "1";
  guiUserScale.name = "User Scale";
  guiUserScale.slotCount = 1;
  guiUserScale.id = "{48F2A59F-789F-4076-AB4B-4CC19B3A929A}";
  guiUserScale.type = FBParamType::Linear;
  guiUserScale.Linear().min = 0.5f;
  guiUserScale.Linear().max = 16.0f;
  auto selectGuiUserScale = [](auto& module) { return &module.userScale; };
  guiUserScale.addrSelector = FFSelectGUIParamAddr(selectGuiModule, selectGuiUserScale);

  return result;
}