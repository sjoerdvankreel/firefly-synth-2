#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule> 
FFMakeOsciFMTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc FM";
  result->slotCount = 1;
  result->id = "{D76EC76F-086E-45D6-9933-0CA0431E35EA}";
  result->params.resize((int)FFOsciFMParam::Count);
  result->addrSelectors.voiceModuleExchange = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.osciFM; });
  auto selectModule = [](auto& state) { return &state.voice.osciFM; };

  auto& mode = result->params[(int)FFOsciFMParam::Mode];
  mode.acc = false;
  mode.name = "Mode";
  mode.slotCount = FFOsciModSlotCount;
  mode.id = "{7347594D-9FBB-426B-A656-5673B6C617F3}";
  mode.type = FBParamType::List;
  mode.List().items = {
    { "{B185E4BE-6832-49D9-B75F-EC979CDF2373}", "Off", "Off"},
    { "{6DD56568-B323-441D-8DBC-6ED52203E877}", "On", "On" },
    { "{C75622B5-DAFD-496E-A76A-2A1CF38D0800}", "TZ", "Through Zero" } };
  mode.slotFormatter = [](int slot) { return FFOsciModMakeSourceAndTargetText(slot); };
  auto selectMode = [](auto& module) { return &module.block.mode; };
  mode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectMode);
  mode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectMode);
  mode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectMode);

  // TODO not linear
  auto& index = result->params[(int)FFOsciFMParam::Index];
  index.acc = true;
  index.defaultText = "0.01";
  index.name = "Index";
  index.slotCount = FFOsciModSlotCount;
  index.unit = "%";
  index.id = "{23E50199-00C9-4427-95EA-8E2A255632E4}";
  index.type = FBParamType::Identity;
  index.slotFormatter = [name = index.name](int slot) { return name + " " + FFOsciModMakeSourceAndTargetText(slot); };
  auto selectIndex = [](auto& module) { return &module.acc.index; };
  index.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectIndex);
  index.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectIndex);
  index.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectIndex);
  index.dependencies.enabled.audio.When({ (int)FFOsciFMParam::Mode }, [](auto const& vs) { return vs[0] != (int)FFOsciFMMode::Off; });

  // TODO not discrete
  auto& delay = result->params[(int)FFOsciFMParam::Delay];
  delay.acc = false;
  delay.name = "Delay";
  delay.slotCount = FFOsciModSlotCount;
  delay.id = "{1B21566A-F1B1-4F8F-87D4-F188E86A0586}";
  delay.type = FBParamType::Discrete;
  delay.Discrete().valueCount = 8;
  delay.slotFormatter = [name = delay.name](int slot) { return name + " " + FFOsciModMakeSourceAndTargetText(slot); };
  auto selectDelay = [](auto& module) { return &module.block.delay; };
  delay.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDelay);
  delay.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDelay);
  delay.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDelay);
  delay.dependencies.enabled.audio.When({ (int)FFOsciFMParam::Mode }, [](auto const& vs) { return vs[0] != (int)FFOsciFMMode::Off; });

  return result;
}