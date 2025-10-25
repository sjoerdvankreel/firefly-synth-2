#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeSettingsTopo(bool isFx)
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Settings";
  result->slotCount = 1;
  result->id = "{E3F0E2B7-436C-4278-8E4F-BE86E3A9A76B}";
  result->params.resize((int)FFSettingsParam::Count);
  result->guiParams.resize((int)FFSettingsGUIParam::Count);
  auto selectGuiModule = [](auto& state) { return &state.settings; }; 
  auto selectModule = [](auto& state) { return &state.global.settings; };

  auto& hostSmoothTime = result->params[(int)FFSettingsParam::HostSmoothTime];
  hostSmoothTime.storeInPatch = false;
  hostSmoothTime.mode = FBParamMode::Block;
  hostSmoothTime.defaultText = "2";
  hostSmoothTime.name = "External MIDI/Automation Smoothing";
  hostSmoothTime.display = "Ext Smth";
  hostSmoothTime.slotCount = 1;
  hostSmoothTime.unit = "Ms";
  hostSmoothTime.id = "{47B38412-40B9-474E-9305-062E7FF7C800}";
  hostSmoothTime.type = FBParamType::Linear;
  hostSmoothTime.Linear().min = 0.0f;
  hostSmoothTime.Linear().max = 0.2f;
  hostSmoothTime.Linear().editSkewFactor = 0.5f;
  hostSmoothTime.Linear().displayMultiplier = 1000.0f;
  auto selectHostSmoothTime = [](auto& module) { return &module.block.hostSmoothTime; };
  hostSmoothTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectHostSmoothTime);

  auto& receiveNotes = result->params[(int)FFSettingsParam::ReceiveNotes];
  receiveNotes.storeInPatch = false;
  receiveNotes.mode = FBParamMode::Block;
  receiveNotes.name = "Receive MIDI Notes";
  receiveNotes.display = "Rcve Notes";
  receiveNotes.slotCount = 1;
  receiveNotes.defaultText = isFx ? "Off" : "On";
  receiveNotes.id = "{92B2E390-F925-4170-BCA0-CFEDBF29970B}";
  receiveNotes.type = FBParamType::Boolean;
  auto selectReceiveNotes = [](auto& module) { return &module.block.receiveNotes; };
  receiveNotes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReceiveNotes);
  receiveNotes.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReceiveNotes);
  receiveNotes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReceiveNotes);

  auto& tuningMode = result->params[(int)FFSettingsParam::TuningMode];
  tuningMode.storeInPatch = false;
  tuningMode.mode = FBParamMode::Block;
  tuningMode.name = "Tuning Mode";
  tuningMode.display = "Tuning";
  tuningMode.slotCount = 1;
  tuningMode.defaultText = "Off";
  tuningMode.id = "{8E17AB52-CC80-4CFA-A575-77D189F31B83}";
  tuningMode.type = FBParamType::List;
  tuningMode.List().items = {
    { "{A29F8B05-2485-4C91-B1E3-8D8A6755604B}", "Off" },
    { "{0D0035E5-3429-48FD-A8AF-E5126B3AB8DE}", "On" },
    { "{28A85D43-1EAE-4573-B20E-EAE3F4CF277C}", "Note" } };
  auto selectTuningMode = [](auto& module) { return &module.block.tuningMode; };
  tuningMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuningMode);
  tuningMode.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuningMode);
  tuningMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuningMode);
  tuningMode.dependencies.enabled.audio.WhenSimple({ (int)FFSettingsParam::TuningMode }, [](auto const& vs) { return false; });

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

  auto& guiHilightMod = result->guiParams[(int)FFSettingsGUIParam::HilightMod];
  guiHilightMod.name = "Hilite Mod";
  guiHilightMod.slotCount = 1;
  guiHilightMod.defaultText = "On";
  guiHilightMod.id = "{193F0F95-D4D2-46A5-A978-C8740E0C08B3}";
  guiHilightMod.type = FBParamType::Boolean;
  auto selectGuiHilightMod = [](auto& module) { return &module.hilightMod; };
  guiHilightMod.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiHilightMod);

  auto& guiHilightTweak = result->guiParams[(int)FFSettingsGUIParam::HilightTweak];
  guiHilightTweak.name = "Hilite Tweak";
  guiHilightTweak.slotCount = 1;
  guiHilightTweak.defaultText = "Off";
  guiHilightTweak.id = "{486DAE85-F8CC-4825-ACEA-D13D3D8A6933}";
  guiHilightTweak.type = FBParamType::Boolean;
  auto selectGuiHilightTweak = [](auto& module) { return &module.hilightTweak; };
  guiHilightTweak.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiHilightTweak);

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