#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>
#include <playground_plug/modules/oscis_graph/FFOscisGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::string
FFOsciModFormatSlot(int slot)
{
  switch (slot)
  {
  case 0: return "1>2";
  case 1: return "1>3";
  case 2: return "2>3";
  case 3: return "1>4";
  case 4: return "2>4";
  case 5: return "3>4";
  default: assert(false); return "";
  }
}

std::unique_ptr<FBStaticModule>
FFMakeOsciModTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc Mod";
  result->slotCount = 1;
  result->graphCount = FFOsciCount;
  result->graphRenderer = FFOscisRenderGraph;
  result->id = "{546F9D74-670A-463D-80B0-B4BBD061DA88}";
  result->params.resize((int)FFOsciModParam::Count);
  result->addrSelectors.voiceModuleExchange = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.osciMod; });
  auto selectModule = [](auto& state) { return &state.voice.osciMod; };

  auto& oversample = result->params[(int)FFOsciModParam::Oversample];
  oversample.acc = false;
  oversample.name = "Oversample";
  oversample.defaultText = "Off";
  oversample.slotCount = 1;
  oversample.id = "{2449CE02-EE0D-48E7-A716-E59633F279FB}";
  oversample.type = FBParamType::Boolean;
  auto selectOversample = [](auto& module) { return &module.block.oversample; };
  oversample.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOversample);
  oversample.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectOversample);
  oversample.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectOversample);

  auto& expoFM = result->params[(int)FFOsciModParam::ExpoFM];
  expoFM.acc = false;
  expoFM.name = "Exp FM";
  expoFM.defaultText = "Off";
  expoFM.slotCount = 1;
  expoFM.id = "{23FC415C-06AD-4ED3-8B29-08724D536096}";
  expoFM.type = FBParamType::Boolean;
  auto selectExpoFM = [](auto& module) { return &module.block.expoFM; };
  expoFM.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectExpoFM);
  expoFM.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectExpoFM);
  expoFM.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectExpoFM);

  auto& amMode = result->params[(int)FFOsciModParam::AMMode];
  amMode.acc = false;
  amMode.name = "AM Mode";
  amMode.slotCount = FFOsciModSlotCount;
  amMode.id = "{AE135DBF-A88E-49A4-9205-08C908E6FC12}";
  amMode.type = FBParamType::List;
  amMode.slotFormatter = FFOsciModFormatSlot;
  amMode.List().items = {
    { "{30807EDC-158F-4356-837E-E0DA1688A576}", "Off" },
    { "{2E046F54-1966-4268-B973-CA6FADB1C50E}", "AM" },
    { "{968AFEB7-5BED-421A-8099-24B6066661EE}", "RM" } };
  auto selectAMMode = [](auto& module) { return &module.block.amMode; };
  amMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectAMMode);
  amMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectAMMode);
  amMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectAMMode);

  auto& amMix = result->params[(int)FFOsciModParam::AMMix];
  amMix.acc = true;
  amMix.defaultText = "1";
  amMix.name = "AM Mix";
  amMix.slotCount = FFOsciModSlotCount;
  amMix.unit = "%";
  amMix.id = "{F0D5C59D-7E40-46A6-AE88-D3B663A675B7}";
  amMix.type = FBParamType::Identity;
  amMix.slotFormatter = FFOsciModFormatSlot;
  auto selectAMMix = [](auto& module) { return &module.acc.amMix; };
  amMix.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectAMMix);
  amMix.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectAMMix);
  amMix.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectAMMix);
  amMix.dependencies.enabled.audio.When({ (int)FFOsciModParam::AMMode }, [](auto const& vs) { return vs[0] != (int)FFOsciModAMMode::Off; });

  auto& fmOn = result->params[(int)FFOsciModParam::FMOn];
  fmOn.acc = false;
  fmOn.name = "FM On";
  fmOn.defaultText = "Off";
  fmOn.slotCount = FFOsciModSlotCount;
  fmOn.id = "{E4F09B87-FBDB-4B72-B6E3-D64BE0733C3D}";
  fmOn.type = FBParamType::Boolean;
  fmOn.slotFormatter = FFOsciModFormatSlot;
  auto selectFMOn = [](auto& module) { return &module.block.fmOn; };
  fmOn.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMOn);
  fmOn.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectFMOn);
  fmOn.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMOn);

  auto& fmIndex = result->params[(int)FFOsciModParam::FMIndex];
  fmIndex.acc = true;
  fmIndex.defaultText = "0";
  fmIndex.name = "FM Index";
  fmIndex.slotCount = FFOsciModSlotCount;
  fmIndex.id = "{9E37D313-1F48-4559-A6DD-01B235240F44}";
  fmIndex.type = FBParamType::Log2;
  fmIndex.Log2().Init(-0.01, 0.01f, 16.01f);
  fmIndex.slotFormatter = FFOsciModFormatSlot;
  auto selectFMIndex = [](auto& module) { return &module.acc.fmIndex; };
  fmIndex.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMIndex);
  fmIndex.dependencies.enabled.audio.When({ (int)FFOsciModParam::FMOn }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}