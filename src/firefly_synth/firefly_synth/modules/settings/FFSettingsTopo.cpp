#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeSettingsTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Settings";
  result->slotCount = 1;
  result->id = "{E3F0E2B7-436C-4278-8E4F-BE86E3A9A76B}";
  result->guiParams.resize((int)FFSettingsGUIParam::Count);
  auto selectGuiModule = [](auto& state) { return &state.settings; }; 

  auto& guiUserScale = result->guiParams[(int)FFSettingsGUIParam::UserScale];
  guiUserScale.unit = "%";
  guiUserScale.defaultText = "1";
  guiUserScale.name = "User Scale";
  guiUserScale.slotCount = 1;
  guiUserScale.id = "{48F2A59F-789F-4076-AB4B-4CC19B3A929A}";
  guiUserScale.type = FBParamType::Linear;
  guiUserScale.Linear().min = 0.5f;
  guiUserScale.Linear().max = 16.0f;
  auto selectGuiUserScale = [](auto& module) { return &module.userScale; };
  guiUserScale.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiUserScale);

  auto& guiKnobVisualsMode = result->guiParams[(int)FFSettingsGUIParam::KnobVisualsMode];
  guiKnobVisualsMode.defaultText = "If Focus";
  guiKnobVisualsMode.name = "Knob Visuals";
  guiKnobVisualsMode.slotCount = 1;
  guiKnobVisualsMode.id = "{75702E18-0499-45ED-935E-A1BC1D0348C3}";
  guiKnobVisualsMode.type = FBParamType::List;
  guiKnobVisualsMode.List().items = {
    { "{74535D7D-90EC-407D-A439-946A9C09351D}", "Basic" },
    { "{C1A672E9-332D-4A33-8FAC-D8D60A892561}", "Always" },
    { "{B2BA8788-B64E-4BF8-8584-DA13916ADC59}", "If Focus" } };
  auto selectGuiKnobVisualsMode = [](auto& module) { return &module.knobVisualsMode; };
  guiKnobVisualsMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiKnobVisualsMode);

  auto& guiGraphVisualsMode = result->guiParams[(int)FFSettingsGUIParam::GraphVisualsMode];
  guiGraphVisualsMode.defaultText = "If Focus";
  guiGraphVisualsMode.name = "Plot Visuals";
  guiGraphVisualsMode.slotCount = 1;
  guiGraphVisualsMode.id = "{6C5F2DC2-C796-444C-8D43-077708580609}";
  guiGraphVisualsMode.type = FBParamType::List;
  guiGraphVisualsMode.List().items = {
    { "{D5C9FC77-0DE8-4077-9D07-073B44F5076F}", "Basic" },
    { "{8291740E-D7DC-4481-B430-9C73F3343E10}", "Always" },
    { "{A5369260-7E9F-4C23-8FED-0C42CAA9DD91}", "If Focus" } };
  auto selectGuiGraphVisualsMode = [](auto& module) { return &module.graphVisualsMode; };
  guiGraphVisualsMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiGraphVisualsMode);

  auto& guiOscSelectedTab = result->guiParams[(int)FFSettingsGUIParam::OscSelectedTab];
  guiOscSelectedTab.defaultText = "0";
  guiOscSelectedTab.name = "Osc Selected Tab";
  guiOscSelectedTab.slotCount = 1;
  guiOscSelectedTab.id = "{677ADE6F-E531-4FA0-AF84-1D31D78AACCB}";
  guiOscSelectedTab.type = FBParamType::Discrete;
  guiOscSelectedTab.Discrete().valueCount = FFOsciCount + 1;
  auto selectGuiOscSelectedTab = [](auto& module) { return &module.oscSelectedTab; };
  guiOscSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiOscSelectedTab);

  auto& guiFxSelectedTab = result->guiParams[(int)FFSettingsGUIParam::FXSelectedTab];
  guiFxSelectedTab.defaultText = "0";
  guiFxSelectedTab.name = "FX Selected Tab";
  guiFxSelectedTab.slotCount = 1;
  guiFxSelectedTab.id = "{EA9332F9-2B1F-4D96-B67C-F632231F7C7B}";
  guiFxSelectedTab.type = FBParamType::Discrete;
  guiFxSelectedTab.Discrete().valueCount = FFEffectCount * 2;
  auto selectGuiFXSelectedTab = [](auto& module) { return &module.fxSelectedTab; };
  guiFxSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiFXSelectedTab);

  auto& guiLFOSelectedTab = result->guiParams[(int)FFSettingsGUIParam::LFOSelectedTab];
  guiLFOSelectedTab.defaultText = "0";
  guiLFOSelectedTab.name = "LFO Selected Tab";
  guiLFOSelectedTab.slotCount = 1;
  guiLFOSelectedTab.id = "{23C0E87A-9224-43BB-97CE-549586B39BF4}";
  guiLFOSelectedTab.type = FBParamType::Discrete;
  guiLFOSelectedTab.Discrete().valueCount = FFLFOCount * 2;
  auto selectGuiLFOSelectedTab = [](auto& module) { return &module.lfoSelectedTab; };
  guiLFOSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiLFOSelectedTab);

  auto& guiMixSelectedTab = result->guiParams[(int)FFSettingsGUIParam::MixSelectedTab];
  guiMixSelectedTab.defaultText = "0";
  guiMixSelectedTab.name = "Mix Selected Tab";
  guiMixSelectedTab.slotCount = 1;
  guiMixSelectedTab.id = "{862452C8-DA0C-4AB6-AB54-2922F8F00E7E}";
  guiMixSelectedTab.type = FBParamType::Discrete;
  guiMixSelectedTab.Discrete().valueCount = 2;
  auto selectGuiMixSelectedTab = [](auto& module) { return &module.mixSelectedTab; };
  guiMixSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiMixSelectedTab);

  auto& guiEnvSelectedTab = result->guiParams[(int)FFSettingsGUIParam::EnvSelectedTab];
  guiEnvSelectedTab.defaultText = "0";
  guiEnvSelectedTab.name = "Env Selected Tab";
  guiEnvSelectedTab.slotCount = 1;
  guiEnvSelectedTab.id = "{83D56B7F-28A7-4A02-AEBB-8D55E6CDB4D8}";
  guiEnvSelectedTab.type = FBParamType::Discrete;
  guiEnvSelectedTab.Discrete().valueCount = FFEnvCount;
  auto selectGuiEnvSelectedTab = [](auto& module) { return &module.envSelectedTab; };
  guiEnvSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiEnvSelectedTab);

  auto& guiEchoSelectedTab = result->guiParams[(int)FFSettingsGUIParam::EchoSelectedTab];
  guiEchoSelectedTab.defaultText = "0";
  guiEchoSelectedTab.name = "Echo Selected Tab";
  guiEchoSelectedTab.slotCount = 1;
  guiEchoSelectedTab.id = "{0087ED9D-5B24-4A22-9F3D-738AF0605136}";
  guiEchoSelectedTab.type = FBParamType::Discrete;
  guiEchoSelectedTab.Discrete().valueCount = 2;
  auto selectGuiEchoSelectedTab = [](auto& module) { return &module.echoSelectedTab; };
  guiEchoSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiEchoSelectedTab);

  return result;
}