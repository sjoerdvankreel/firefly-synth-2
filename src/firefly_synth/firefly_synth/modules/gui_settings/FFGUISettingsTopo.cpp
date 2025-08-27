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
  result->slotCount = 1;
  result->id = "{2407B76A-7FB3-4BD6-B6FD-B1F610AF8147}";
  result->params.resize((int)FFGUISettingsParam::Count);
  result->guiParams.resize((int)FFGUISettingsGUIParam::Count);
  auto selectGuiModule = [](auto& state) { return &state.guiSettings; }; 
  auto selectModule = [](auto& state) { return &state.global.guiSettings; };

  auto& guiUserScale = result->guiParams[(int)FFGUISettingsGUIParam::UserScale];
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

  auto& guiShowMatrix = result->guiParams[(int)FFGUISettingsGUIParam::ShowMatrix];
  guiShowMatrix.defaultText = "Off";
  guiShowMatrix.name = "Show Matrix";
  guiShowMatrix.slotCount = 1;
  guiShowMatrix.id = "{FEB66217-BE68-4B94-A8D3-009EE307BBB5}";
  guiShowMatrix.type = FBParamType::Boolean;
  auto selectGuiShowMatrix = [](auto& module) { return &module.showMatrix; };
  guiShowMatrix.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiShowMatrix);

  auto& guiGraphRenderMode = result->guiParams[(int)FFGUISettingsGUIParam::GraphRenderMode];
  guiGraphRenderMode.defaultText = "If Focus";
  guiGraphRenderMode.name = "Graph";
  guiGraphRenderMode.slotCount = 1;
  guiGraphRenderMode.id = "{6C5F2DC2-C796-444C-8D43-077708580609}";
  guiGraphRenderMode.type = FBParamType::List;
  guiGraphRenderMode.List().items = {
    { "{D5C9FC77-0DE8-4077-9D07-073B44F5076F}", "Basic" },
    { "{8291740E-D7DC-4481-B430-9C73F3343E10}", "Always" },
    { "{A5369260-7E9F-4C23-8FED-0C42CAA9DD91}", "If Focus" } };
  auto selectGuiGraphRenderMode = [](auto& module) { return &module.graphRenderMode; };
  guiGraphRenderMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiGraphRenderMode);

  auto& guiKnobRenderMode = result->guiParams[(int)FFGUISettingsGUIParam::KnobRenderMode];
  guiKnobRenderMode.defaultText = "If Focus";
  guiKnobRenderMode.name = "Knob Mod";
  guiKnobRenderMode.slotCount = 1;
  guiKnobRenderMode.id = "{586DE12C-6D53-48C0-B5EE-CBAE92985465}";
  guiKnobRenderMode.type = FBParamType::List;
  guiKnobRenderMode.List().items = {
    { "{28EDDEC0-2CB0-422F-B786-8804EE311F43}", "Never" },
    { "{F682B536-34F4-4BAC-90A9-CF23E23BE0B1}", "Always" },
    { "{F6DA89D0-A8FE-42F3-A7F2-C83FCFE6DE19}", "If Focus" } };
  auto selectGuiKnobRenderMode = [](auto& module) { return &module.knobRenderMode; };
  guiKnobRenderMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiKnobRenderMode);

  auto& guiOscSelectedTab = result->guiParams[(int)FFGUISettingsGUIParam::OscSelectedTab];
  guiOscSelectedTab.defaultText = "0";
  guiOscSelectedTab.name = "Osc Selected Tab";
  guiOscSelectedTab.slotCount = 1;
  guiOscSelectedTab.id = "{677ADE6F-E531-4FA0-AF84-1D31D78AACCB}";
  guiOscSelectedTab.type = FBParamType::Discrete;
  guiOscSelectedTab.Discrete().valueCount = FFOsciCount + 1;
  auto selectGuiOscSelectedTab = [](auto& module) { return &module.oscSelectedTab; };
  guiOscSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiOscSelectedTab);

  auto& guiFxSelectedTab = result->guiParams[(int)FFGUISettingsGUIParam::FXSelectedTab];
  guiFxSelectedTab.defaultText = "0";
  guiFxSelectedTab.name = "FX Selected Tab";
  guiFxSelectedTab.slotCount = 1;
  guiFxSelectedTab.id = "{EA9332F9-2B1F-4D96-B67C-F632231F7C7B}";
  guiFxSelectedTab.type = FBParamType::Discrete;
  guiFxSelectedTab.Discrete().valueCount = FFEffectCount * 2;
  auto selectGuiFXSelectedTab = [](auto& module) { return &module.fxSelectedTab; };
  guiFxSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiFXSelectedTab);

  auto& guiLFOSelectedTab = result->guiParams[(int)FFGUISettingsGUIParam::LFOSelectedTab];
  guiLFOSelectedTab.defaultText = "0";
  guiLFOSelectedTab.name = "LFO Selected Tab";
  guiLFOSelectedTab.slotCount = 1;
  guiLFOSelectedTab.id = "{23C0E87A-9224-43BB-97CE-549586B39BF4}";
  guiLFOSelectedTab.type = FBParamType::Discrete;
  guiLFOSelectedTab.Discrete().valueCount = FFLFOCount * 2;
  auto selectGuiLFOSelectedTab = [](auto& module) { return &module.lfoSelectedTab; };
  guiLFOSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiLFOSelectedTab);

  auto& guiMixSelectedTab = result->guiParams[(int)FFGUISettingsGUIParam::MixSelectedTab];
  guiMixSelectedTab.defaultText = "0";
  guiMixSelectedTab.name = "Mix Selected Tab";
  guiMixSelectedTab.slotCount = 1;
  guiMixSelectedTab.id = "{862452C8-DA0C-4AB6-AB54-2922F8F00E7E}";
  guiMixSelectedTab.type = FBParamType::Discrete;
  guiMixSelectedTab.Discrete().valueCount = 2;
  auto selectGuiMixSelectedTab = [](auto& module) { return &module.mixSelectedTab; };
  guiMixSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiMixSelectedTab);

  auto& guiEnvSelectedTab = result->guiParams[(int)FFGUISettingsGUIParam::EnvSelectedTab];
  guiEnvSelectedTab.defaultText = "0";
  guiEnvSelectedTab.name = "Env Selected Tab";
  guiEnvSelectedTab.slotCount = 1;
  guiEnvSelectedTab.id = "{83D56B7F-28A7-4A02-AEBB-8D55E6CDB4D8}";
  guiEnvSelectedTab.type = FBParamType::Discrete;
  guiEnvSelectedTab.Discrete().valueCount = FFEnvCount;
  auto selectGuiEnvSelectedTab = [](auto& module) { return &module.envSelectedTab; };
  guiEnvSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiEnvSelectedTab);

  auto& guiEchoSelectedTab = result->guiParams[(int)FFGUISettingsGUIParam::EchoSelectedTab];
  guiEchoSelectedTab.defaultText = "0";
  guiEchoSelectedTab.name = "Echo Selected Tab";
  guiEchoSelectedTab.slotCount = 1;
  guiEchoSelectedTab.id = "{0087ED9D-5B24-4A22-9F3D-738AF0605136}";
  guiEchoSelectedTab.type = FBParamType::Discrete;
  guiEchoSelectedTab.Discrete().valueCount = 2;
  auto selectGuiEchoSelectedTab = [](auto& module) { return &module.echoSelectedTab; };
  guiEchoSelectedTab.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiEchoSelectedTab);

  // dummy which we check on the audio if it changed
  auto& flushDelayToggle = result->params[(int)FFGUISettingsParam::FlushDelayToggle];
  flushDelayToggle.acc = false;
  flushDelayToggle.thisIsNotARealParameter = true;
  flushDelayToggle.name = "Flush Delay";
  flushDelayToggle.slotCount = 1;
  flushDelayToggle.defaultText = "Off";
  flushDelayToggle.id = "{22F4FB2F-BAD8-43A0-BC28-88F5F3A3B7CF}";
  flushDelayToggle.type = FBParamType::Boolean;
  auto selectFlushDelayToggle = [](auto& module) { return &module.block.flushDelayToggle; };
  flushDelayToggle.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFlushDelayToggle);
  flushDelayToggle.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFlushDelayToggle);
  flushDelayToggle.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFlushDelayToggle);

  return result;
}