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
  result->name = "GUI Settings";
  result->slotCount = 1;
  result->id = "{E3F0E2B7-436C-4278-8E4F-BE86E3A9A76B}";
  result->guiParams.resize((int)FFGUISettingsGUIParam::Count);
  auto selectGuiModule = [](auto& state) { return &state.guiSettings; }; 
    
  auto& guiUserScale = result->guiParams[(int)FFGUISettingsGUIParam::UserScale];
  guiUserScale.unit = "%"; 
  guiUserScale.defaultText = "1";
  guiUserScale.name = "User Scale";
  guiUserScale.slotCount = 1;
  guiUserScale.id = "{48F2A59F-789F-4076-AB4B-4CC19B3A929A}";
  guiUserScale.description = "User Scale";
  guiUserScale.type = FBParamType::Linear;
  guiUserScale.Linear().min = 0.5f;
  guiUserScale.Linear().max = 4.0f;
  auto selectGuiUserScale = [](auto& module) { return &module.userScale; };
  guiUserScale.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiUserScale);

  auto& guiHilightMod = result->guiParams[(int)FFGUISettingsGUIParam::HilightMod];
  guiHilightMod.name = "Show Mod Bounds";
  guiHilightMod.slotCount = 1;
  guiHilightMod.defaultText = "On";
  guiHilightMod.id = "{193F0F95-D4D2-46A5-A978-C8740E0C08B3}";
  guiHilightMod.description = "Highlight Parameter Modulation Bounds";
  guiHilightMod.type = FBParamType::Boolean;
  auto selectGuiHilightMod = [](auto& module) { return &module.hilightMod; };
  guiHilightMod.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiHilightMod);
   
  auto& guiHilightTweakMode = result->guiParams[(int)FFGUISettingsGUIParam::HilightTweakMode];
  guiHilightTweakMode.name = "Show Tweaked From";
  guiHilightTweakMode.slotCount = 1;
  guiHilightTweakMode.defaultText = "Off";
  guiHilightTweakMode.id = "{486DAE85-F8CC-4825-ACEA-D13D3D8A6933}"; 
  guiHilightTweakMode.description = "Highlight Tweaked Parameters";
  guiHilightTweakMode.type = FBParamType::List;
  guiHilightTweakMode.List().items = {
   { "{C89F3D84-0C5E-4130-9663-FE372973FBCE}", "Off" },
   { "{40441318-0851-4652-BE7E-1227ACD20B84}", "Patch" },
   { "{5A4E565C-BFD9-4227-A926-3B690A23B7C0}", "Session" },
   { "{FF071331-4BBC-4925-A8A6-64E368A3FA00}", "Default" } };
  auto selectGuiHilightTweakMode = [](auto& module) { return &module.hilightTweakMode; };
  guiHilightTweakMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiHilightTweakMode);

  auto& guiKnobVisualsMode = result->guiParams[(int)FFGUISettingsGUIParam::KnobVisualsMode];
  guiKnobVisualsMode.defaultText = "If Focus";
  guiKnobVisualsMode.name = "Engine Knob Visuals";
  guiKnobVisualsMode.slotCount = 1;
  guiKnobVisualsMode.id = "{75702E18-0499-45ED-935E-A1BC1D0348C3}";
  guiKnobVisualsMode.description = "Highlight Engine Parameter State";
  guiKnobVisualsMode.type = FBParamType::List;
  guiKnobVisualsMode.List().items = {
    { "{74535D7D-90EC-407D-A439-946A9C09351D}", "Off" },
    { "{C1A672E9-332D-4A33-8FAC-D8D60A892561}", "On" },
    { "{B2BA8788-B64E-4BF8-8584-DA13916ADC59}", "If Focus" } };
  auto selectGuiKnobVisualsMode = [](auto& module) { return &module.knobVisualsMode; };
  guiKnobVisualsMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiKnobVisualsMode);

  auto& guiGraphVisualsMode = result->guiParams[(int)FFGUISettingsGUIParam::GraphVisualsMode];
  guiGraphVisualsMode.defaultText = "If Focus";
  guiGraphVisualsMode.name = "Engine Plot Visuals";
  guiGraphVisualsMode.slotCount = 1;
  guiGraphVisualsMode.id = "{6C5F2DC2-C796-444C-8D43-077708580609}";
  guiGraphVisualsMode.description = "Show Engine State In Graph Plots";
  guiGraphVisualsMode.type = FBParamType::List;
  guiGraphVisualsMode.List().items = {
    { "{D5C9FC77-0DE8-4077-9D07-073B44F5076F}", "Off" },
    { "{8291740E-D7DC-4481-B430-9C73F3343E10}", "On" },
    { "{A5369260-7E9F-4C23-8FED-0C42CAA9DD91}", "If Focus" } };
  auto selectGuiGraphVisualsMode = [](auto& module) { return &module.graphVisualsMode; };
  guiGraphVisualsMode.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiGraphVisualsMode);

  return result;
}