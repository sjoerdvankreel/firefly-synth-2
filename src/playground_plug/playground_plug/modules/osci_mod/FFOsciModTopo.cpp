#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

static std::string
FormatSlot(int slot)
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
  result->id = "{546F9D74-670A-463D-80B0-B4BBD061DA88}";
  result->params.resize((int)FFOsciModParam::Count);
  result->addrSelectors.voiceModuleExchange = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.osciMod; });
  auto selectModule = [](auto& state) { return &state.voice.osciMod; };

  auto& on = result->params[(int)FFOsciModParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = FFOsciModSlotCount;
  on.id = "{83FC9C98-CF1B-4E5A-91DB-2A6488B73809}";
  on.type = FBParamType::Boolean;
  on.slotFormatter = FormatSlot;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& am = result->params[(int)FFOsciModParam::AM];
  am.acc = true;
  am.defaultText = "0";
  am.name = "AM";
  am.slotCount = FFOsciModSlotCount;
  am.unit = "%";
  am.id = "{4D04D0E5-3749-428C-B300-9151177555F3}";
  am.type = FBParamType::Identity;
  am.slotFormatter = [name = am.name](int slot) { return name + " " + FormatSlot(slot); };
  auto selectAM = [](auto& module) { return &module.acc.am; };
  am.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectAM);
  am.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectAM);
  am.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectAM);
  am.dependencies.enabled.audio.When({ (int)FFOsciModParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& rm = result->params[(int)FFOsciModParam::RM];
  rm.acc = true;
  rm.defaultText = "0";
  rm.name = "RM";
  rm.slotCount = FFOsciModSlotCount;
  rm.unit = "%";
  rm.id = "{4D04D0E5-3749-428C-B300-9151177555F3}";
  rm.type = FBParamType::Identity;
  rm.slotFormatter = [name = rm.name](int slot) { return name + " " + FormatSlot(slot); };
  auto selectRM = [](auto& module) { return &module.acc.rm; };
  rm.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectRM);
  rm.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectRM);
  rm.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectRM);
  rm.dependencies.enabled.audio.When({ (int)FFOsciModParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& fm = result->params[(int)FFOsciModParam::FM];
  fm.acc = true;
  fm.defaultText = "0";
  fm.name = "FM";
  fm.slotCount = FFOsciModSlotCount;
  fm.id = "{3F224B8F-5AB3-48D5-8925-1D9207AFA3B9}";
  fm.type = FBParamType::Log2;
  fm.Log2().Init(-0.01, 0.01f, 1.01f);
  fm.slotFormatter = [name = fm.name](int slot) { return name + " " + FormatSlot(slot); };
  auto selectFM = [](auto& module) { return &module.acc.fm; };
  fm.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFM);
  fm.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFM);
  fm.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFM);
  fm.dependencies.enabled.audio.When({ (int)FFOsciModParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& tzfm = result->params[(int)FFOsciModParam::TZFM];
  tzfm.acc = true;
  tzfm.defaultText = "0";
  tzfm.name = "TZFM";
  tzfm.slotCount = FFOsciModSlotCount;
  tzfm.id = "{4A0DE9FA-A8BC-4B2F-9FB6-584F7C353CE2}";
  tzfm.type = FBParamType::Log2;
  tzfm.Log2().Init(-0.01, 0.01f, 1.01f);
  tzfm.slotFormatter = [name = tzfm.name](int slot) { return name + " " + FormatSlot(slot); };
  auto selectTZFM = [](auto& module) { return &module.acc.tzfm; };
  tzfm.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectTZFM);
  tzfm.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectTZFM);
  tzfm.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectTZFM);
  tzfm.dependencies.enabled.audio.When({ (int)FFOsciModParam::On }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}