#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mix/FFGMixTopo.hpp>
#include <firefly_synth/modules/mix/FFGMixState.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::string
FormatVoiceToGFXSlot(FBStaticTopo const&, int mixSlot)
{
  std::string fxName = "FX " + std::to_string(mixSlot + 1);
  return std::string("Voice\U00002192" + fxName);
}

std::unique_ptr<FBStaticModule>
FFMakeGMixTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GMix";
  result->slotCount = 1;
  result->id = "{09ED2CCE-3AC5-455E-935C-A684BEBC154D}";
  result->params.resize((int)FFGMixParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gMix; });
  auto selectModule = [](auto& state) { return &state.global.gMix; };

  auto& amp = result->params[(int)FFGMixParam::Amp];
  amp.acc = true;
  amp.defaultText = "50";
  amp.name = "Amp";
  amp.slotCount = 1;
  amp.unit = "%";
  amp.id = "{7A7248F3-570F-4AB4-B4FE-CA7D1B8531CF}";
  amp.type = FBParamType::Linear;
  amp.Linear().min = 0.0f;
  amp.Linear().max = 2.0f;
  amp.Linear().displayMultiplier = 100;
  auto selectAmp = [](auto& module) { return &module.acc.amp; };
  amp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAmp);
  amp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAmp);
  amp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAmp);

  auto& lfo1ToAmp = result->params[(int)FFGMixParam::LFO1ToAmp];
  lfo1ToAmp.acc = true;
  lfo1ToAmp.name = "GLFO 1\U00002192Amp";
  lfo1ToAmp.defaultText = "0";
  lfo1ToAmp.slotCount = 1;
  lfo1ToAmp.unit = "%";
  lfo1ToAmp.id = "{EF9C4C3E-C0F8-4050-BE77-DAFFFEC31756}";
  lfo1ToAmp.type = FBParamType::Identity;
  auto selectLFO1ToAmp = [](auto& module) { return &module.acc.lfo1ToAmp; };
  lfo1ToAmp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLFO1ToAmp);
  lfo1ToAmp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectLFO1ToAmp);
  lfo1ToAmp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLFO1ToAmp);

  auto& bal = result->params[(int)FFGMixParam::Bal];
  bal.acc = true;
  bal.defaultText = "0";
  bal.name = "Balance";
  bal.display = "Bal";
  bal.slotCount = 1;
  bal.unit = "%";
  bal.id = "{C85CEDAD-917D-444A-967C-6D9FCEA1828E}";
  bal.type = FBParamType::Linear;
  bal.Linear().displayMultiplier = 100;
  bal.Linear().min = -1.0f;
  bal.Linear().max = 1.0f;
  auto selectBal = [](auto& module) { return &module.acc.bal; };
  bal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectBal);
  bal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectBal);
  bal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectBal);

  auto& lfo2ToBal = result->params[(int)FFGMixParam::LFO2ToBal];
  lfo2ToBal.acc = true;
  lfo2ToBal.name = "GLFO 2\U00002192Bal";
  lfo2ToBal.defaultText = "0";
  lfo2ToBal.slotCount = 1;
  lfo2ToBal.unit = "%";
  lfo2ToBal.id = "{B4A71A47-EA18-416C-89CC-38C66A2A17E5}";
  lfo2ToBal.type = FBParamType::Identity;
  auto selectLFO2ToBal = [](auto& module) { return &module.acc.lfo2ToBal; };
  lfo2ToBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLFO2ToBal);
  lfo2ToBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectLFO2ToBal);
  lfo2ToBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLFO2ToBal);

  auto& voiceToGFX = result->params[(int)FFGMixParam::VoiceToGFX];
  voiceToGFX.acc = true;
  voiceToGFX.defaultText = "0";
  voiceToGFX.name = "Voice To GFX";
  voiceToGFX.slotCount = FFEffectCount;
  voiceToGFX.unit = "%";
  voiceToGFX.id = "{43E24F38-ADA0-41A0-88BD-B17333ABFA9C}";
  voiceToGFX.slotFormatter = FormatVoiceToGFXSlot;
  voiceToGFX.slotFormatterOverrides = true;
  voiceToGFX.type = FBParamType::Identity;
  auto selectVoiceToGFX = [](auto& module) { return &module.acc.voiceToGFX; };
  voiceToGFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoiceToGFX);
  voiceToGFX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectVoiceToGFX);
  voiceToGFX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoiceToGFX);

  auto& gfxToGFX = result->params[(int)FFGMixParam::GFXToGFX];
  gfxToGFX.acc = true;
  gfxToGFX.defaultText = "0";
  gfxToGFX.name = "GFX To GFX";
  gfxToGFX.slotCount = FFMixFXToFXCount;
  gfxToGFX.unit = "%";
  gfxToGFX.id = "{4F4C11F4-94B9-4685-B44F-70BCC96B1F3C}";
  gfxToGFX.slotFormatter = FFMixFormatFXToFXSlot;
  gfxToGFX.slotFormatterOverrides = true;
  gfxToGFX.type = FBParamType::Identity;
  auto selectGFXToGFX = [](auto& module) { return &module.acc.GFXToGFX; };
  gfxToGFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGFXToGFX);
  gfxToGFX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectGFXToGFX);
  gfxToGFX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGFXToGFX);

  auto& voiceToOut = result->params[(int)FFGMixParam::VoiceToOut];
  voiceToOut.acc = true;
  voiceToOut.defaultTextSelector = [](int /*mi*/, int /*ms*/, int ps) { return ps == 0 ? "100" : "0"; };
  voiceToOut.name = "Voice\U00002192Out";
  voiceToOut.slotCount = 1;
  voiceToOut.unit = "%";
  voiceToOut.id = "{72FCC170-A112-401E-BF11-C91A59C26457}";
  voiceToOut.type = FBParamType::Identity;
  auto selectVoiceToOut = [](auto& module) { return &module.acc.voiceToOut; };
  voiceToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoiceToOut);
  voiceToOut.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectVoiceToOut);
  voiceToOut.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoiceToOut);

  auto& gfxToOut = result->params[(int)FFGMixParam::GFXToOut];
  gfxToOut.acc = true;
  gfxToOut.defaultText = "0";
  gfxToOut.name = "GFX To Out";
  gfxToOut.slotCount = FFEffectCount;
  gfxToOut.unit = "%";
  gfxToOut.id = "{009211F4-37C6-4FE6-B563-F2636A7C3587}";
  gfxToOut.slotFormatter = FFMixFormatFXToOutSlot;
  gfxToOut.slotFormatterOverrides = true;
  gfxToOut.type = FBParamType::Identity;
  auto selectGFXToOut = [](auto& module) { return &module.acc.GFXToOut; };
  gfxToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGFXToOut);
  gfxToOut.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectGFXToOut);
  gfxToOut.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGFXToOut);

  return result;
}