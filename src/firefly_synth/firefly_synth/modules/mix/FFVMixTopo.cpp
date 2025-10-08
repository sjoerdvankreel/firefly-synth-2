#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mix/FFVMixTopo.hpp>
#include <firefly_synth/modules/mix/FFVMixState.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::string
FormatOsciMixToOut(FBStaticTopo const& /*topo*/, int /* moduleSlot */, int /*mixSlot*/)
{
  return "Osc Mix\U00002192Out";
}

static std::string
FormatOsciToOutSlot(FBStaticTopo const& topo, int /* moduleSlot */, int mixSlot)
{
  int osciSlot = mixSlot;
  std::string osciName = topo.modules[(int)FFModuleType::Osci].name + " " + std::to_string(osciSlot + 1);
  return osciName + "\U00002192Out";
}

static std::string
FormatOsciToOsciMixSlot(FBStaticTopo const& topo, int /* moduleSlot */, int mixSlot)
{
  int osciSlot = mixSlot;
  std::string osciName = topo.modules[(int)FFModuleType::Osci].name + " " + std::to_string(osciSlot + 1);
  return osciName + "\U00002192Osc Mix";
}

static std::string
FormatOsciMixToVFXSlot(FBStaticTopo const& /*topo*/, int /* moduleSlot */, int mixSlot)
{
  int fxSlot = mixSlot;
  std::string fxName = "VFX " + std::to_string(fxSlot + 1);
  return "Osc Mix\U00002192" + fxName;
}

static std::string
FormatOsciToVFXSlot(FBStaticTopo const& topo, int /* moduleSlot */, int mixSlot)
{
  int fxSlot = FFVMixOsciToVFXGetFXSlot(mixSlot);
  int osciSlot = FFVMixOsciToVFXGetOsciSlot(mixSlot);
  std::string fxName = "VFX " + std::to_string(fxSlot + 1);
  std::string osciName = topo.modules[(int)FFModuleType::Osci].name + " " + std::to_string(osciSlot + 1);
  return osciName + "\U00002192" + fxName;
}

std::unique_ptr<FBStaticModule>
FFMakeVMixTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "VMix";
  result->slotCount = 1;
  result->id = "{5C91D5A0-3EC1-4142-935A-3FE83B60C04E}";
  result->params.resize((int)FFVMixParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vMix; });
  auto selectModule = [](auto& state) { return &state.voice.vMix; };

  auto& amp = result->params[(int)FFVMixParam::Amp];
  amp.mode = FBParamMode::Accurate;
  amp.defaultText = "100";
  amp.name = "Amp";
  amp.slotCount = 1;
  amp.unit = "%";
  amp.id = "{02B3BE49-9ECC-4289-9488-CAB4252B6E9D}";
  amp.type = FBParamType::Linear;
  amp.Linear().min = 0.0f;
  amp.Linear().max = 2.0f;
  amp.Linear().displayMultiplier = 100;
  auto selectAmp = [](auto& module) { return &module.acc.amp; };
  amp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAmp);
  amp.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectAmp);
  amp.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAmp);

  auto& ampEnvToAmp = result->params[(int)FFVMixParam::AmpEnvToAmp];
  ampEnvToAmp.mode = FBParamMode::Accurate;
  ampEnvToAmp.name = "Amp Env\U00002192Amp";
  ampEnvToAmp.defaultText = "100";
  ampEnvToAmp.slotCount = 1;
  ampEnvToAmp.unit = "%";
  ampEnvToAmp.id = "{6C0E8516-778A-47FB-BDC5-0A6F132C13B8}";
  ampEnvToAmp.type = FBParamType::Identity;
  auto selectAmpEnvToAmp = [](auto& module) { return &module.acc.ampEnvToAmp; };
  ampEnvToAmp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAmpEnvToAmp);
  ampEnvToAmp.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectAmpEnvToAmp);
  ampEnvToAmp.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAmpEnvToAmp);

  auto& ampEnvTarget = result->params[(int)FFVMixParam::AmpEnvTarget];
  ampEnvTarget.mode = FBParamMode::Block;
  ampEnvTarget.slotCount = 1;
  ampEnvTarget.name = "Amp Env Target";
  ampEnvTarget.display = "Target";
  ampEnvTarget.defaultText = "VMix Out";
  ampEnvTarget.id = "{B1D76321-C513-4836-9AB6-741E2204D128}";
  ampEnvTarget.type = FBParamType::List;
  ampEnvTarget.List().items = {
    { "{350E31BF-0F89-4C30-B719-9E436F2074C2}", "Off" },
    { "{1DAA2CE0-67ED-4A09-A5E6-A907F101F4A7}", "Osc PreMix" },
    { "{60E614A3-1FB8-4158-8884-37393464F52D}", "Osc PostMix" },
    { "{94FDB0DE-84D2-4DE0-98E5-AE247F337CED}", "Osc Mix" },
    { "{6FBC9845-FEDB-467D-9EAB-FB924DE10877}", "VFX In" },
    { "{D2E4DE41-7CB6-411E-BF5E-08970EAB3124}", "VFX Out" },
    { "{5D47DEB7-B2CF-4F98-BCD6-523CD0F9130A}", "VMix In" },
    { "{88AA06B7-BFCB-463B-9C6D-21A619BE6FB4}", "VMix Out" } };

  auto selectAmpEnvTarget = [](auto& module) { return &module.block.ampEnvTarget; };
  ampEnvTarget.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAmpEnvTarget);
  ampEnvTarget.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectAmpEnvTarget);
  ampEnvTarget.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAmpEnvTarget);

  auto& bal = result->params[(int)FFVMixParam::Bal];
  bal.mode = FBParamMode::Accurate;
  bal.defaultText = "0";
  bal.name = "Balance";
  bal.display = "Bal";
  bal.slotCount = 1;
  bal.unit = "%";
  bal.id = "{51623C27-E29C-4877-B76D-D123B0D13F45}";
  bal.type = FBParamType::Linear;
  bal.Linear().displayMultiplier = 100;
  bal.Linear().min = -1.0f;
  bal.Linear().max = 1.0f;
  auto selectBal = [](auto& module) { return &module.acc.bal; };
  bal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectBal);
  bal.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectBal);
  bal.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectBal); 

  auto& lfo6ToBal = result->params[(int)FFVMixParam::LFO6ToBal];
  lfo6ToBal.mode = FBParamMode::Accurate;
  lfo6ToBal.name = "VLFO 6\U00002192Bal";
  lfo6ToBal.defaultText = "0";
  lfo6ToBal.slotCount = 1;
  lfo6ToBal.unit = "%";
  lfo6ToBal.id = "{1B17DA72-4E2E-4D47-91C4-B9858ED85640}";
  lfo6ToBal.type = FBParamType::Identity;
  auto selectLFO6ToBal = [](auto& module) { return &module.acc.lfo6ToBal; };
  lfo6ToBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLFO6ToBal);
  lfo6ToBal.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectLFO6ToBal);
  lfo6ToBal.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLFO6ToBal);

  auto& osciToOsciMix = result->params[(int)FFVMixParam::OsciToOsciMix];
  osciToOsciMix.mode = FBParamMode::Accurate;
  osciToOsciMix.defaultTextSelector = [](int /*mi*/, int /*ms*/, int ps) { return ps == 0 ? "100" : "0"; };
  osciToOsciMix.name = "Osc To Osc Mix";
  osciToOsciMix.slotCount = FFOsciCount;
  osciToOsciMix.unit = "%";
  osciToOsciMix.id = "{3EF9D095-1F5E-45BD-8B9E-55EEC321AA43}";
  osciToOsciMix.slotFormatter = FormatOsciToOsciMixSlot;
  osciToOsciMix.slotFormatterOverrides = true;
  osciToOsciMix.type = FBParamType::Identity;
  auto selectOsciToOsciMix = [](auto& module) { return &module.acc.osciToOsciMix; };
  osciToOsciMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciToOsciMix);
  osciToOsciMix.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciToOsciMix);
  osciToOsciMix.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciToOsciMix);

  auto& osciMixToVFX = result->params[(int)FFVMixParam::OsciMixToVFX];
  osciMixToVFX.mode = FBParamMode::Accurate;
  osciMixToVFX.defaultText = "0";
  osciMixToVFX.name = "Osc Mix To VFX";
  osciMixToVFX.slotCount = FFEffectCount;
  osciMixToVFX.unit = "%";
  osciMixToVFX.id = "{54582A91-57BD-4F24-BA01-390AF0339EB2}";
  osciMixToVFX.slotFormatter = FormatOsciMixToVFXSlot;
  osciMixToVFX.slotFormatterOverrides = true;
  osciMixToVFX.type = FBParamType::Identity;
  auto selectOsciMixToVFX = [](auto& module) { return &module.acc.osciMixToVFX; };
  osciMixToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciMixToVFX);
  osciMixToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciMixToVFX);
  osciMixToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciMixToVFX);
  
  auto& osciToVFX = result->params[(int)FFVMixParam::OsciToVFX];
  osciToVFX.mode = FBParamMode::Accurate;
  osciToVFX.defaultText = "0";
  osciToVFX.name = "Osc To VFX";
  osciToVFX.slotCount = FFVMixOsciToVFXCount;
  osciToVFX.unit = "%";
  osciToVFX.id = "{1BC03120-9E8C-412B-81D4-17CC662BA72B}";
  osciToVFX.slotFormatter = FormatOsciToVFXSlot;
  osciToVFX.slotFormatterOverrides = true;
  osciToVFX.type = FBParamType::Identity;
  auto selectOsciToVFX = [](auto& module) { return &module.acc.osciToVFX; };
  osciToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciToVFX);
  osciToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciToVFX);
  osciToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciToVFX);

  auto& vfxToVFX = result->params[(int)FFVMixParam::VFXToVFX];
  vfxToVFX.mode = FBParamMode::Accurate;
  vfxToVFX.defaultText = "0";
  vfxToVFX.name = "VFX To VFX";
  vfxToVFX.slotCount = FFMixFXToFXCount;
  vfxToVFX.unit = "%";
  vfxToVFX.id = "{21EF058C-86B5-4E7B-B4A1-5CFE92F20065}";
  vfxToVFX.slotFormatter = [](auto const& topo, int moduleSlot, int mixSlot) { return FFMixFormatFXToFXSlot(topo, false, moduleSlot, mixSlot); };
  vfxToVFX.slotFormatterOverrides = true;
  vfxToVFX.type = FBParamType::Identity;
  auto selectVFXToVFX = [](auto& module) { return &module.acc.VFXToVFX; };
  vfxToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVFXToVFX);
  vfxToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectVFXToVFX);
  vfxToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVFXToVFX);  

  auto& osciToOut = result->params[(int)FFVMixParam::OsciToOut];
  osciToOut.mode = FBParamMode::Accurate;
  osciToOut.defaultText = "0";
  osciToOut.name = "Osc To Out";
  osciToOut.slotCount = FFOsciCount;
  osciToOut.unit = "%";
  osciToOut.id = "{FFBE0515-CF42-4E5F-B453-0CEA58A623D8}";
  osciToOut.slotFormatter = FormatOsciToOutSlot;
  osciToOut.slotFormatterOverrides = true;
  osciToOut.type = FBParamType::Identity;
  auto selectOsciToOut = [](auto& module) { return &module.acc.osciToOut; };
  osciToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciToOut);
  osciToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciToOut);
  osciToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciToOut);

  auto& vfxToOut = result->params[(int)FFVMixParam::VFXToOut];
  vfxToOut.mode = FBParamMode::Accurate;
  vfxToOut.defaultText = "0";
  vfxToOut.name = "VFX To Out";
  vfxToOut.slotCount = FFEffectCount;
  vfxToOut.unit = "%";
  vfxToOut.id = "{D159D4DD-BF49-4208-BEAE-D5BE550AB9FA}";
  vfxToOut.slotFormatter = [](auto const& topo, int moduleSlot, int mixSlot) { return FFMixFormatFXToOutSlot(topo, false, moduleSlot, mixSlot); };
  vfxToOut.slotFormatterOverrides = true;
  vfxToOut.type = FBParamType::Identity;
  auto selectVFXToOut = [](auto& module) { return &module.acc.VFXToOut; };
  vfxToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVFXToOut);
  vfxToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectVFXToOut);
  vfxToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVFXToOut);

  auto& osciMixToOut = result->params[(int)FFVMixParam::OsciMixToOut];
  osciMixToOut.mode = FBParamMode::Accurate;
  osciMixToOut.defaultText = "100";
  osciMixToOut.name = "Osc Mix To Out";
  osciMixToOut.slotCount = 1;
  osciMixToOut.unit = "%";
  osciMixToOut.id = "{44FCA1C1-297B-4EE9-B928-81F2CF31EB2A}";
  osciMixToOut.slotFormatter = FormatOsciMixToOut;
  osciMixToOut.slotFormatterOverrides = true;
  osciMixToOut.type = FBParamType::Identity;
  auto selectOsciMixToOut = [](auto& module) { return &module.acc.osciMixToOut; };
  osciMixToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciMixToOut);
  osciMixToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciMixToOut);
  osciMixToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciMixToOut);

  return result;
}