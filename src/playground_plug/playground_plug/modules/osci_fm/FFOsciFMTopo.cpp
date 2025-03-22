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

  auto& on = result->params[(int)FFOsciFMParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = FFOsciModSlotCount;
  on.id = "{7347594D-9FBB-426B-A656-5673B6C617F3}";
  on.type = FBParamType::Boolean;
  on.slotFormatter = [](int slot) { return FFOsciModMakeSourceAndTargetText(slot); };
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& throughZero = result->params[(int)FFOsciFMParam::ThroughZero];
  throughZero.acc = false;
  throughZero.defaultText = "On";
  throughZero.name = "TZ";
  throughZero.tooltip = "Through-Zero";
  throughZero.slotCount = FFOsciModSlotCount;
  throughZero.id = "{99BC9193-3D60-4EEE-BFE8-17C0E39971B7}";
  throughZero.type = FBParamType::Boolean;
  throughZero.slotFormatter = [tooltip = throughZero.tooltip](int slot) { return tooltip + " " + FFOsciModMakeSourceAndTargetText(slot); };
  auto selectThroughZero = [](auto& module) { return &module.block.throughZero; };
  throughZero.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectThroughZero);
  throughZero.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectThroughZero);
  throughZero.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectThroughZero);
  throughZero.dependencies.enabled.audio.When({ (int)FFOsciFMParam::On }, [](auto const& vs) { return vs[0] != 0; });

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
  index.dependencies.enabled.audio.When({ (int)FFOsciFMParam::On }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}