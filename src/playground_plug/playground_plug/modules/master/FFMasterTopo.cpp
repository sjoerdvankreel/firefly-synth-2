#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeMasterTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Master";
  result->slotCount = 1;
  result->id = "{83AA98D4-9D12-4D61-81A4-4FAA935EDF5D}";
  result->params.resize((int)FFMasterParam::Count);
  auto selectModule = [](auto& state) { return &state.global.master; };

  auto& gain = result->params[(int)FFMasterParam::Gain];
  gain.acc = true;
  gain.defaultText = "33";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.unit = "%";
  gain.id = "{9CDC04BC-D0FF-43E6-A2C2-D6C822CFA3EA}";
  gain.type = FBParamType::Linear;
  gain.linear.displayMultiplier = 100.0f;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.addrSelectors.globalAccProc = FFSelectProcParamAddr(selectModule, selectGain);
  gain.addrSelectors.globalExchange = FFSelectExchangeParamAddr(selectModule, selectGain);

  auto& hostSmoothTime = result->params[(int)FFMasterParam::HostSmoothTime];
  hostSmoothTime.acc = false;
  hostSmoothTime.defaultText = "2";
  hostSmoothTime.name = "Smth";
  hostSmoothTime.slotCount = 1;
  hostSmoothTime.unit = "Ms";
  hostSmoothTime.id = "{961D1B53-9509-47EA-B646-C948C5FACA82}";
  hostSmoothTime.type = FBParamType::Linear;
  hostSmoothTime.linear.min = 0.0f;
  hostSmoothTime.linear.max = 0.05f;
  hostSmoothTime.linear.displayMultiplier = 1000.0f;
  auto selectHostSmoothTime = [](auto& module) { return &module.block.hostSmoothTime; };
  hostSmoothTime.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.addrSelectors.globalBlockProc = FFSelectProcParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.addrSelectors.globalExchange = FFSelectExchangeParamAddr(selectModule, selectHostSmoothTime);

  auto& voices = result->params[(int)FFMasterParam::Voices];
  voices.acc = false;
  voices.output = true;
  voices.defaultText = "0";
  voices.name = "Voices";
  voices.slotCount = 1;
  voices.id = "{54C6CAD4-FA5A-4395-8131-C768E0AD48CF}";
  voices.type = FBParamType::Discrete;
  voices.discrete.valueCount = FBMaxVoices + 1;
  auto selectVoices = [](auto& module) { return &module.block.voices; };
  voices.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectVoices);
  voices.addrSelectors.globalBlockProc = FFSelectProcParamAddr(selectModule, selectVoices);
  voices.addrSelectors.globalExchange = FFSelectExchangeParamAddr(selectModule, selectVoices);

  return result;
}