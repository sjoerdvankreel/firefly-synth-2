#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mix/FFGMixTopo.hpp>
#include <firefly_synth/modules/mix/FFGMixState.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::string
FormatVoiceMixToGFXSlot(FBStaticTopo const&, int /* moduleSlot */, int mixSlot)
{
  std::string fxName = "GFX " + std::to_string(mixSlot + 1);
  return std::string("VMix\U00002192" + fxName);
}

static std::string
FormatAudioInToGFXSlot(FBStaticTopo const&, int /* moduleSlot */, int mixSlot)
{
  std::string fxName = "GFX " + std::to_string(mixSlot + 1);
  return std::string("Audio In\U00002192" + fxName);
}

static std::string
FormatSidechainToGFXSlot(FBStaticTopo const&, int /* moduleSlot */, int mixSlot)
{
  std::string fxName = "GFX " + std::to_string(mixSlot + 1);
  return std::string("Sidechain\U00002192" + fxName);
}

std::unique_ptr<FBStaticModule>
FFMakeGMixTopo(bool isFx)
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GMix";
  result->slotCount = 1;
  result->id = "{09ED2CCE-3AC5-455E-935C-A684BEBC154D}";
  result->params.resize((int)FFGMixParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gMix; });
  auto selectModule = [](auto& state) { return &state.global.gMix; };

  // need something to tie the audio in visibility to
  auto& dummyToggle = result->params[(int)FFGMixParam::DummyToggle];
  dummyToggle.mode = FBParamMode::Fake;
  dummyToggle.defaultText = "Off";
  dummyToggle.name = "Dummy Toggle";
  dummyToggle.description = "Dummy Toggle";
  dummyToggle.slotCount = 1;
  dummyToggle.id = "{C1EB5E5F-D4E4-43CC-B2B6-6BB3E1D79296}";
  dummyToggle.type = FBParamType::Boolean;
  auto selectDummyToggle = [](auto& module) { return &module.block.dummyToggle; };
  dummyToggle.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDummyToggle);
  dummyToggle.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDummyToggle);
  dummyToggle.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDummyToggle);

  auto& amp = result->params[(int)FFGMixParam::Amp];
  amp.mode = FBParamMode::Accurate;
  amp.defaultText = "50";
  amp.name = "Amp";
  amp.slotCount = 1;
  amp.unit = "%";
  amp.id = "{7A7248F3-570F-4AB4-B4FE-CA7D1B8531CF}";
  amp.description = "Global Amplitude";
  amp.type = FBParamType::Linear;
  amp.Linear().min = 0.0f;
  amp.Linear().max = 2.0f;
  amp.Linear().displayMultiplier = 100;
  auto selectAmp = [](auto& module) { return &module.acc.amp; };
  amp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAmp);
  amp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAmp);
  amp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAmp);

  auto& lfo5ToAmp = result->params[(int)FFGMixParam::LFO5ToAmp];
  lfo5ToAmp.mode = FBParamMode::Accurate;
  lfo5ToAmp.name = "GLFO 5\U00002192Amp";
  lfo5ToAmp.defaultText = "0";
  lfo5ToAmp.slotCount = 1;
  lfo5ToAmp.unit = "%";
  lfo5ToAmp.id = "{EF9C4C3E-C0F8-4050-BE77-DAFFFEC31756}";
  lfo5ToAmp.description = "Modulate Global Amplitude By GLFO 5";
  lfo5ToAmp.type = FBParamType::Identity;
  auto selectLFO5ToAmp = [](auto& module) { return &module.acc.lfo5ToAmp; };
  lfo5ToAmp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLFO5ToAmp);
  lfo5ToAmp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectLFO5ToAmp);
  lfo5ToAmp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLFO5ToAmp);

  auto& bal = result->params[(int)FFGMixParam::Bal];
  bal.mode = FBParamMode::Accurate;
  bal.defaultText = "0";
  bal.name = "Bal";
  bal.slotCount = 1;
  bal.unit = "%";
  bal.id = "{C85CEDAD-917D-444A-967C-6D9FCEA1828E}";
  bal.description = "Global Stereo Balance";
  bal.type = FBParamType::Linear;
  bal.Linear().displayMultiplier = 100;
  bal.Linear().min = -1.0f;
  bal.Linear().max = 1.0f;
  auto selectBal = [](auto& module) { return &module.acc.bal; };
  bal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectBal);
  bal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectBal);
  bal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectBal);

  auto& lfo6ToBal = result->params[(int)FFGMixParam::LFO6ToBal];
  lfo6ToBal.mode = FBParamMode::Accurate;
  lfo6ToBal.name = "GLFO 6\U00002192Bal";
  lfo6ToBal.defaultText = "0";
  lfo6ToBal.slotCount = 1;
  lfo6ToBal.unit = "%";
  lfo6ToBal.id = "{B4A71A47-EA18-416C-89CC-38C66A2A17E5}";
  lfo6ToBal.description = "Modulate Global Stereo Balance By GLFO 6";
  lfo6ToBal.type = FBParamType::Identity;
  auto selectLFO6ToBal = [](auto& module) { return &module.acc.lfo6ToBal; };
  lfo6ToBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLFO6ToBal);
  lfo6ToBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectLFO6ToBal);
  lfo6ToBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLFO6ToBal);

  auto& voiceToGFX = result->params[(int)FFGMixParam::VoiceToGFX];
  voiceToGFX.mode = FBParamMode::Accurate;
  voiceToGFX.defaultText = "0";
  voiceToGFX.name = "VMix To GFX";
  voiceToGFX.slotCount = FFEffectCount;
  voiceToGFX.unit = "%";
  voiceToGFX.id = "{43E24F38-ADA0-41A0-88BD-B17333ABFA9C}";
  voiceToGFX.description = "Route Voice Mixdown To Global FX";
  voiceToGFX.slotFormatter = FormatVoiceMixToGFXSlot;
  voiceToGFX.slotFormatterOverrides = true;
  voiceToGFX.type = FBParamType::Identity;
  auto selectVoiceToGFX = [](auto& module) { return &module.acc.voiceToGFX; };
  voiceToGFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoiceToGFX);
  voiceToGFX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectVoiceToGFX);
  voiceToGFX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoiceToGFX);

  auto& audioInToGFX = result->params[(int)FFGMixParam::AudioInToGFX];
  audioInToGFX.mode = FBParamMode::Accurate;
  audioInToGFX.defaultText = "0";
  audioInToGFX.name = "Audio In To GFX";
  audioInToGFX.slotCount = FFEffectCount;
  audioInToGFX.unit = "%";
  audioInToGFX.id = "{20FB04D5-30A4-4AE8-9D8B-7CF039DC5A12}";
  audioInToGFX.description = "Route Audio In To Global FX";
  audioInToGFX.slotFormatter = FormatAudioInToGFXSlot;
  audioInToGFX.slotFormatterOverrides = true;
  audioInToGFX.type = FBParamType::Identity;
  auto selectAudioInToGFX = [](auto& module) { return &module.acc.audioInToGFX; };
  audioInToGFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAudioInToGFX);
  audioInToGFX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAudioInToGFX);
  audioInToGFX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAudioInToGFX);
  audioInToGFX.dependencies.visible.audio.WhenSimple({ (int)FFGMixParam::DummyToggle }, [isFx](auto const&) { return isFx; });

  auto& sidechainToGFX = result->params[(int)FFGMixParam::SidechainToGFX];
  sidechainToGFX.mode = FBParamMode::Accurate;
  sidechainToGFX.defaultText = "0";
  sidechainToGFX.name = "Sidechain To GFX";
  sidechainToGFX.slotCount = FFEffectCount;
  sidechainToGFX.unit = "%";
  sidechainToGFX.id = "{00DC72E9-88D5-4156-BB38-D394CBE9C40B}";
  sidechainToGFX.description = "Route Sidechain To Global FX";
  sidechainToGFX.slotFormatter = FormatSidechainToGFXSlot;
  sidechainToGFX.slotFormatterOverrides = true;
  sidechainToGFX.type = FBParamType::Identity;
  auto selectSidechainToGFX = [](auto& module) { return &module.acc.sidechainToGFX; };
  sidechainToGFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSidechainToGFX);
  sidechainToGFX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectSidechainToGFX);
  sidechainToGFX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSidechainToGFX);

  auto& gfxToGFX = result->params[(int)FFGMixParam::GFXToGFX];
  gfxToGFX.mode = FBParamMode::Accurate;
  gfxToGFX.defaultText = "0";
  gfxToGFX.name = "GFX To GFX";
  gfxToGFX.slotCount = FFMixFXToFXCount;
  gfxToGFX.unit = "%";
  gfxToGFX.id = "{4F4C11F4-94B9-4685-B44F-70BCC96B1F3C}";
  gfxToGFX.description = "Route Global FX To Global FX";
  gfxToGFX.slotFormatter = [](auto const& topo, int moduleSlot, int mixSlot) { return FFMixFormatFXToFXSlot(topo, true, moduleSlot, mixSlot); };
  gfxToGFX.slotFormatterOverrides = true;
  gfxToGFX.type = FBParamType::Identity;
  auto selectGFXToGFX = [](auto& module) { return &module.acc.GFXToGFX; };
  gfxToGFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGFXToGFX);
  gfxToGFX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectGFXToGFX);
  gfxToGFX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGFXToGFX);

  auto& voiceToOut = result->params[(int)FFGMixParam::VoiceToOut];
  voiceToOut.mode = FBParamMode::Accurate;
  voiceToOut.defaultText = isFx ? "0" : "100";
  voiceToOut.name = "VMix\U00002192Out";
  voiceToOut.slotCount = 1;
  voiceToOut.unit = "%";
  voiceToOut.id = "{72FCC170-A112-401E-BF11-C91A59C26457}";
  voiceToOut.description = "Route Voice Mixdown To Master Output";
  voiceToOut.type = FBParamType::Identity;
  auto selectVoiceToOut = [](auto& module) { return &module.acc.voiceToOut; };
  voiceToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoiceToOut);
  voiceToOut.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectVoiceToOut);
  voiceToOut.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoiceToOut);

  auto& audioInToOut = result->params[(int)FFGMixParam::AudioInToOut];
  audioInToOut.mode = FBParamMode::Accurate;
  audioInToOut.defaultText = isFx? "100": "0";
  audioInToOut.name = "Audio In\U00002192Out";
  audioInToOut.slotCount = 1;
  audioInToOut.unit = "%";
  audioInToOut.id = "{5037D6CD-CB68-42E5-AD48-2B3BA8FAB213}";
  audioInToOut.description = "Route Audio In To Master Output";
  audioInToOut.type = FBParamType::Identity;
  auto selectAudioInToOut = [](auto& module) { return &module.acc.audioInToOut; };
  audioInToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAudioInToOut);
  audioInToOut.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAudioInToOut);
  audioInToOut.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAudioInToOut);
  audioInToOut.dependencies.visible.audio.WhenSimple({ (int)FFGMixParam::DummyToggle }, [isFx](auto const&) { return isFx; });

  auto& sidechainToOut = result->params[(int)FFGMixParam::SidechainToOut];
  sidechainToOut.mode = FBParamMode::Accurate;
  sidechainToOut.defaultText = "0";
  sidechainToOut.name = "Sidechain\U00002192Out";
  sidechainToOut.slotCount = 1;
  sidechainToOut.unit = "%";
  sidechainToOut.id = "{A6D11F6A-716C-40F5-8B3A-117581D4D64E}";
  sidechainToOut.description = "Route Sidechain To Master Output";
  sidechainToOut.type = FBParamType::Identity;
  auto selectSidechainToOut = [](auto& module) { return &module.acc.sidechainToOut; };
  sidechainToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSidechainToOut);
  sidechainToOut.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectSidechainToOut);
  sidechainToOut.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSidechainToOut);

  auto& gfxToOut = result->params[(int)FFGMixParam::GFXToOut];
  gfxToOut.mode = FBParamMode::Accurate;
  gfxToOut.defaultText = "0";
  gfxToOut.name = "GFX To Out";
  gfxToOut.slotCount = FFEffectCount;
  gfxToOut.unit = "%";
  gfxToOut.id = "{009211F4-37C6-4FE6-B563-F2636A7C3587}";
  gfxToOut.description = "Route Global FX To Master Output";
  gfxToOut.slotFormatter = [](auto const& topo, int moduleSlot, int mixSlot) { return FFMixFormatFXToOutSlot(topo, true, moduleSlot, mixSlot); };
  gfxToOut.slotFormatterOverrides = true;
  gfxToOut.type = FBParamType::Identity;
  auto selectGFXToOut = [](auto& module) { return &module.acc.GFXToOut; };
  gfxToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGFXToOut);
  gfxToOut.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectGFXToOut);
  gfxToOut.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGFXToOut);

  return result;
}