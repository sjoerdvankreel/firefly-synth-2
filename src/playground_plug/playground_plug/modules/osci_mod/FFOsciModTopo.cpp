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

  auto& oversampling = result->params[(int)FFOsciModParam::Oversampling];
  oversampling.acc = false;
  oversampling.name = "4X Oversampling";
  oversampling.slotCount = 1;
  oversampling.id = "{2449CE02-EE0D-48E7-A716-E59633F279FB}";
  oversampling.type = FBParamType::Boolean;
  auto selectOversampling = [](auto& module) { return &module.block.oversampling; };
  oversampling.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOversampling);
  oversampling.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectOversampling);
  oversampling.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectOversampling);

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

  auto& fmMode = result->params[(int)FFOsciModParam::FMMode];
  fmMode.acc = false;
  fmMode.name = "FM Mode";
  fmMode.slotCount = FFOsciModSlotCount;
  fmMode.id = "{E4F09B87-FBDB-4B72-B6E3-D64BE0733C3D}";
  fmMode.type = FBParamType::List;
  fmMode.slotFormatter = FFOsciModFormatSlot;
  fmMode.List().items = {
    { "{8B789678-8C69-445F-B7D6-B999D88714B1}", "Off" },
    { "{FAEE7133-DB2D-4E67-9F8D-B1E0F8E87EEA}", "FM" },
    { "{CD270554-42CF-4779-A2A2-219FD26986C3}", "TZ" } };
  auto selectFMMode = [](auto& module) { return &module.block.fmMode; };
  fmMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMMode);
  fmMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectFMMode);
  fmMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMMode);

  auto& fmIndex = result->params[(int)FFOsciModParam::FMIndex];
  fmIndex.acc = true;
  fmIndex.defaultText = "1";
  fmIndex.name = "FM Index";
  fmIndex.slotCount = FFOsciModSlotCount;
  fmIndex.id = "{9E37D313-1F48-4559-A6DD-01B235240F44}";
  fmIndex.type = FBParamType::Linear;
  fmIndex.Linear().min = 0.0f;
  fmIndex.Linear().max = 64.0f;
  fmIndex.slotFormatter = FFOsciModFormatSlot;
  auto selectFMIndex = [](auto& module) { return &module.acc.fmIndex; };
  fmIndex.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMIndex);
  fmIndex.dependencies.enabled.audio.When({ (int)FFOsciModParam::FMMode }, [](auto const& vs) { return vs[0] != (int)FFOsciModFMMode::Off; });

  return result;
}