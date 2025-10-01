#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGraph.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::string
FFGlobalUniTargetToString(FFGlobalUniTarget target)
{
  switch (target)
  {
  case FFGlobalUniTarget::VoiceCoarse: return "Voice Coarse";
  case FFGlobalUniTarget::VoiceFine: return "Voice Fine";
  case FFGlobalUniTarget::VMixAmp: return "VMix Amp";
  case FFGlobalUniTarget::VMixBal: return "VMix Bal";
  case FFGlobalUniTarget::OscGain: return "Osc Gain";
  case FFGlobalUniTarget::OscPan: return "Osc Pan";
  case FFGlobalUniTarget::OscCoarse: return "Osc Coarse";
  case FFGlobalUniTarget::OscFine: return "Osc Fine";
  case FFGlobalUniTarget::OscPhaseOffset: return "Osc Phase";
  case FFGlobalUniTarget::LFORate: return "VLFO Rate";
  case FFGlobalUniTarget::LFOMin: return "VLFO Min";
  case FFGlobalUniTarget::LFOMax: return "VLFO Max";
  case FFGlobalUniTarget::LFOSkewAX: return "VLFO SkewA X";
  case FFGlobalUniTarget::LFOSkewAY: return "VLFO SkewA Y";
  case FFGlobalUniTarget::LFOPhaseOffset: return "VLFO Phase";
  case FFGlobalUniTarget::EnvOffset: return "Env Offset";
  case FFGlobalUniTarget::EnvStretch: return "Env Stretch";
  case FFGlobalUniTarget::VFXParamA: return "VFX Param A";
  case FFGlobalUniTarget::VFXParamB: return "VFX Param B";
  case FFGlobalUniTarget::VFXParamC: return "VFX Param C";
  case FFGlobalUniTarget::VFXParamD: return "VFX Param D";
  case FFGlobalUniTarget::EchoExtend: return "VEcho Extend";
  case FFGlobalUniTarget::EchoFade: return "VEcho Fade";
  case FFGlobalUniTarget::EchoTapLevel: return "VEcho Tap Lvl";
  case FFGlobalUniTarget::EchoTapDelay: return "VEcho Tap Dly";
  case FFGlobalUniTarget::EchoTapBal: return "VEcho Tap Bal";
  case FFGlobalUniTarget::EchoFdbkDelay: return "VEcho Fdbk Dly";
  case FFGlobalUniTarget::EchoFdbkMix: return "VEcho Fdbk Mix";
  case FFGlobalUniTarget::EchoFdbkAmt: return "VEcho Fdbk Amt";
  case FFGlobalUniTarget::EchoReverbMix: return "VEcho Rvrb Mix";
  case FFGlobalUniTarget::EchoReverbSize: return "VEcho Rvrb Size";
  case FFGlobalUniTarget::EchoReverbDamp: return "VEcho Rvrb Damp";
  default: FB_ASSERT(false); return "";
  }
}

static std::string
ManualSlotFormatter(FFGlobalUniTarget target, int slot)
{
  return "Manual " + FFGlobalUniTargetToString(target) + " " + std::to_string(slot + 1);
}

static void
SetManualParamEnabledWhen(FBStaticParam& param, FFGlobalUniTarget target)
{
  param.dependencies.enabled.audio.WhenSlots({ 
    { (int)FFGlobalUniParam::VoiceCount, -1 }, 
    { (int)FFGlobalUniParam::ManualFirst + (int)target, -1 }, 
    { (int)FFGlobalUniParam::Mode, (int)target } 
  }, [](auto const& slots, auto const& vs) { 
      return slots[1] < vs[0] && vs[2] == (int)FFGlobalUniMode::Manual; 
  });
}

std::unique_ptr<FBStaticModule>
FFMakeGlobalUniTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Global Uni";
  result->matrixName = "G.Uni";
  result->slotCount = 1;
  result->graphCount = 1;
  result->graphParticipatesInMain = false;
  result->graphRenderer = FFGlobalUniRenderGraph;
  result->id = "{5A505AF6-4E05-4680-BAFE-B880CE4E1BC2}";
  result->params.resize((int)FFGlobalUniParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.globalUni; });
  auto selectModule = [](auto& state) { return &state.global.globalUni; };

  auto& voiceCount = result->params[(int)FFGlobalUniParam::VoiceCount];
  voiceCount.mode = FBParamMode::Block;
  voiceCount.defaultText = "1";
  voiceCount.name = "Voices";
  voiceCount.slotCount = 1;
  voiceCount.id = "{617F995E-38CC-40BE-899E-AEAE37852092}";
  voiceCount.type = FBParamType::Discrete;
  voiceCount.Discrete().valueCount = FFGlobalUniMaxCount;
  voiceCount.Discrete().valueOffset = 1;
  auto selectVoiceCount = [](auto& module) { return &module.block.voiceCount; };
  voiceCount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoiceCount);
  voiceCount.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectVoiceCount);
  voiceCount.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoiceCount);

  auto& mode = result->params[(int)FFGlobalUniParam::Mode];
  mode.mode = FBParamMode::Block;
  mode.defaultText = "Off";
  mode.name = "Mode";
  mode.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Mode"; };
  mode.slotFormatterOverrides = true;
  mode.slotCount = (int)FFGlobalUniTarget::Count;
  mode.id = "{B5809A8A-B0A9-40B2-8A0B-413121869836}";
  mode.type = FBParamType::List;
  mode.List().items = {
    { "{328055DD-795F-402B-9B16-F30589866295}", "Off" },
    { "{770E5F05-0041-4750-805F-BF08A5135B1B}", "Auto" },
    { "{7645E42A-1249-4483-9019-4F92AD9D0FF7}", "Manual" } };
  auto selectMode = [](auto& module) { return &module.block.mode; };
  mode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectMode);
  mode.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectMode);
  mode.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectMode);
  
  auto& autoSpread = result->params[(int)FFGlobalUniParam::AutoSpread];
  autoSpread.mode = FBParamMode::Accurate;
  autoSpread.defaultText = "0";
  autoSpread.name = "Sprd";
  autoSpread.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Spread"; };
  autoSpread.slotFormatterOverrides = true;
  autoSpread.slotCount = (int)FFGlobalUniTarget::Count;
  autoSpread.id = "{6048E64F-F600-49AF-9DE6-16FB77134A40}";
  autoSpread.unit = "%";
  autoSpread.type = FBParamType::Identity;
  auto selectAutoSpread = [](auto& module) { return &module.acc.autoSpread; };
  autoSpread.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoSpread);
  autoSpread.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAutoSpread);
  autoSpread.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoSpread);
  autoSpread.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode } }, [](auto const& vs) { return vs[0] == (int)FFGlobalUniMode::Auto; });

  auto& autoSpace = result->params[(int)FFGlobalUniParam::AutoSpace];
  autoSpace.mode = FBParamMode::Accurate;
  autoSpace.defaultText = "0";
  autoSpace.name = "Space";
  autoSpace.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Space"; };
  autoSpace.slotFormatterOverrides = true;
  autoSpace.slotCount = (int)FFGlobalUniTarget::Count;
  autoSpace.id = "{F406EEC6-0A27-4486-BDE3-42947993DFE8}";
  autoSpace.unit = "%";
  autoSpace.type = FBParamType::Identity;
  autoSpace.Identity().displayAsBipolar = true;
  auto selectAutoSpace = [](auto& module) { return &module.acc.autoSpace; };
  autoSpace.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoSpace);
  autoSpace.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAutoSpace);
  autoSpace.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoSpace);
  autoSpace.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode } }, [](auto const& vs) { return vs[0] == (int)FFGlobalUniMode::Auto; });

  auto& autoRand = result->params[(int)FFGlobalUniParam::AutoRand];
  autoRand.mode = FBParamMode::VoiceStart;
  autoRand.defaultText = "0";
  autoRand.name = "Rand";
  autoRand.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Rand"; };
  autoRand.slotFormatterOverrides = true;
  autoRand.slotCount = (int)FFGlobalUniTarget::Count;
  autoRand.id = "{74DDA592-503E-4A8E-A026-EB15A3977D7B}";
  autoRand.unit = "%";
  autoRand.type = FBParamType::Identity;
  auto selectAutoRand = [](auto& module) { return &module.acc.autoRand; };
  autoRand.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoRand);
  autoRand.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAutoRand);
  autoRand.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoRand);
  autoRand.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode } }, [](auto const& vs) { return vs[0] == (int)FFGlobalUniMode::Auto; });

  auto& autoRandSeed = result->params[(int)FFGlobalUniParam::AutoRandSeed];
  autoRandSeed.mode = FBParamMode::Block;
  autoRandSeed.defaultText = "0";
  autoRandSeed.name = "Seed";
  autoRandSeed.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Seed"; };
  autoRandSeed.slotFormatterOverrides = true;
  autoRandSeed.slotCount = (int)FFGlobalUniTarget::Count;
  autoRandSeed.id = "{3D9A110B-B3F4-4700-B1A6-D3F5F7CEA368}";
  autoRandSeed.type = FBParamType::Discrete;
  autoRandSeed.Discrete().valueCount = FFGlobalUniMaxSeed + 1;
  auto selectAutoRandSeed = [](auto& module) { return &module.block.autoRandSeed; };
  autoRandSeed.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoRandSeed);
  autoRandSeed.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectAutoRandSeed);
  autoRandSeed.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoRandSeed);
  autoRandSeed.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode } }, [](auto const& vs) { return vs[0] == (int)FFGlobalUniMode::Auto; });

  auto& autoRandFree = result->params[(int)FFGlobalUniParam::AutoRandFree];
  autoRandFree.mode = FBParamMode::Block;
  autoRandFree.defaultText = "Off";
  autoRandFree.name = "Free";
  autoRandFree.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Free"; };
  autoRandFree.slotFormatterOverrides = true;
  autoRandFree.slotCount = (int)FFGlobalUniTarget::Count;
  autoRandFree.id = "{8ED21529-ABEF-4D1C-BA27-768B0016BEC9}";
  autoRandFree.type = FBParamType::Boolean;
  auto selectAutoRandFree = [](auto& module) { return &module.block.autoRandFree; };
  autoRandFree.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoRandFree);
  autoRandFree.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectAutoRandFree);
  autoRandFree.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoRandFree);
  autoRandFree.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode } }, [](auto const& vs) { return vs[0] == (int)FFGlobalUniMode::Auto; });

  auto& manualVoiceCoarse = result->params[(int)FFGlobalUniParam::ManualVoiceCoarse];
  manualVoiceCoarse.mode = FBParamMode::Accurate;
  manualVoiceCoarse.defaultText = "0";
  manualVoiceCoarse.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VoiceCoarse, s); };
  manualVoiceCoarse.slotFormatterOverrides = true;
  manualVoiceCoarse.slotCount = FFGlobalUniMaxCount;
  manualVoiceCoarse.unit = "Semitones";
  manualVoiceCoarse.id = "{0D24BD6A-DE67-4CE6-8272-BC290D5D7D2D}";
  manualVoiceCoarse.type = FBParamType::Linear;
  manualVoiceCoarse.Linear().min = -FFGlobalUniCoarseSemis;
  manualVoiceCoarse.Linear().max = FFGlobalUniCoarseSemis;
  auto selectManualVoiceCoarse = [](auto& module) { return &module.acc.manualVoiceCoarse; };
  manualVoiceCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVoiceCoarse);
  manualVoiceCoarse.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVoiceCoarse);
  manualVoiceCoarse.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVoiceCoarse);
  SetManualParamEnabledWhen(manualVoiceCoarse, FFGlobalUniTarget::VoiceCoarse);

  auto& manualVoiceFine = result->params[(int)FFGlobalUniParam::ManualVoiceFine];
  manualVoiceFine.mode = FBParamMode::Accurate;
  manualVoiceFine.defaultText = "0";
  manualVoiceFine.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VoiceFine, s); };
  manualVoiceFine.slotFormatterOverrides = true;
  manualVoiceFine.slotCount = FFGlobalUniMaxCount;
  manualVoiceFine.id = "{4A3335C3-9239-4D90-A53A-F73807FEC0B7}";
  manualVoiceFine.unit = "%";
  manualVoiceFine.type = FBParamType::Identity;
  manualVoiceFine.Identity().displayAsBipolar = true;
  auto selectManualVoiceFine = [](auto& module) { return &module.acc.manualVoiceFine; };
  manualVoiceFine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVoiceFine);
  manualVoiceFine.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVoiceFine);
  manualVoiceFine.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVoiceFine);
  SetManualParamEnabledWhen(manualVoiceFine, FFGlobalUniTarget::VoiceFine);

  auto& manualVMixAmp = result->params[(int)FFGlobalUniParam::ManualVMixAmp];
  manualVMixAmp.mode = FBParamMode::Accurate;
  manualVMixAmp.defaultText = "100";
  manualVMixAmp.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VMixAmp, s); };
  manualVMixAmp.slotFormatterOverrides = true;
  manualVMixAmp.slotCount = FFGlobalUniMaxCount;
  manualVMixAmp.id = "{CA16FDA4-CB83-46F4-B236-5382DAA1CFD6}";
  manualVMixAmp.unit = "%";
  manualVMixAmp.type = FBParamType::Identity;
  auto selectManualVMixAmp = [](auto& module) { return &module.acc.manualVMixAmp; };
  manualVMixAmp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVMixAmp);
  manualVMixAmp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVMixAmp);
  manualVMixAmp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVMixAmp);
  SetManualParamEnabledWhen(manualVMixAmp, FFGlobalUniTarget::VMixAmp);

  auto& manualVMixBal = result->params[(int)FFGlobalUniParam::ManualVMixBal];
  manualVMixBal.mode = FBParamMode::Accurate;
  manualVMixBal.defaultText = "0";
  manualVMixBal.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VMixBal, s); };
  manualVMixBal.slotFormatterOverrides = true;
  manualVMixBal.slotCount = FFGlobalUniMaxCount;
  manualVMixBal.id = "{A6AD53AB-3FBD-4579-9B2C-D97118B16E70}";
  manualVMixBal.unit = "%";
  manualVMixBal.type = FBParamType::Identity;
  manualVMixBal.Identity().displayAsBipolar = true;
  auto selectManualVMixBal = [](auto& module) { return &module.acc.manualVMixBal; };
  manualVMixBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVMixBal);
  manualVMixBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVMixBal);
  manualVMixBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVMixBal);
  SetManualParamEnabledWhen(manualVMixBal, FFGlobalUniTarget::VMixBal);

  auto& manualOscGain = result->params[(int)FFGlobalUniParam::ManualOscGain];
  manualOscGain.mode = FBParamMode::Accurate;
  manualOscGain.defaultText = "100";
  manualOscGain.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscGain, s); };
  manualOscGain.slotFormatterOverrides = true;
  manualOscGain.slotCount = FFGlobalUniMaxCount;
  manualOscGain.unit = "%";
  manualOscGain.id = "{847E5FD5-8DAC-43B1-BA0E-125FD916844C}";
  manualOscGain.type = FBParamType::Identity;
  auto selectManualOscGain = [](auto& module) { return &module.acc.manualOscGain; };
  manualOscGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualOscGain);
  manualOscGain.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualOscGain);
  manualOscGain.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualOscGain);
  SetManualParamEnabledWhen(manualOscGain, FFGlobalUniTarget::OscGain);

  auto& manualOscPan = result->params[(int)FFGlobalUniParam::ManualOscPan];
  manualOscPan.mode = FBParamMode::Accurate;
  manualOscPan.defaultText = "0";
  manualOscPan.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscPan, s); };
  manualOscPan.slotFormatterOverrides = true;
  manualOscPan.slotCount = FFGlobalUniMaxCount;
  manualOscPan.unit = "%";
  manualOscPan.id = "{9CACBCF5-9D85-431B-986B-E2B50DC3336A}";
  manualOscPan.type = FBParamType::Identity;
  manualOscPan.Identity().displayAsBipolar = true;
  auto selectManualOscPan = [](auto& module) { return &module.acc.manualOscPan; };
  manualOscPan.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualOscPan);
  manualOscPan.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualOscPan);
  manualOscPan.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualOscPan);
  SetManualParamEnabledWhen(manualOscPan, FFGlobalUniTarget::OscPan);

  auto& manualOscCoarse = result->params[(int)FFGlobalUniParam::ManualOscCoarse];
  manualOscCoarse.mode = FBParamMode::Accurate;
  manualOscCoarse.defaultText = "0";
  manualOscCoarse.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscCoarse, s); };
  manualOscCoarse.slotFormatterOverrides = true;
  manualOscCoarse.slotCount = FFGlobalUniMaxCount;
  manualOscCoarse.unit = "Semitones";
  manualOscCoarse.id = "{C16C50D5-00C0-4393-87F9-1A7C457F4483}";
  manualOscCoarse.type = FBParamType::Linear;
  manualOscCoarse.Linear().min = -FFGlobalUniCoarseSemis;
  manualOscCoarse.Linear().max = FFGlobalUniCoarseSemis;
  auto selectmanualOscCoarse = [](auto& module) { return &module.acc.manualOscCoarse; };
  manualOscCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectmanualOscCoarse);
  manualOscCoarse.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectmanualOscCoarse);
  manualOscCoarse.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectmanualOscCoarse);
  SetManualParamEnabledWhen(manualOscCoarse, FFGlobalUniTarget::OscCoarse);

  auto& manualOscFine = result->params[(int)FFGlobalUniParam::ManualOscFine];
  manualOscFine.mode = FBParamMode::Accurate;
  manualOscFine.defaultText = "0";
  manualOscFine.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscFine, s); };
  manualOscFine.slotFormatterOverrides = true;
  manualOscFine.slotCount = FFGlobalUniMaxCount;
  manualOscFine.id = "{D32C05E9-BE89-43CB-887F-AF7B1001518F}";
  manualOscFine.unit = "%";
  manualOscFine.type = FBParamType::Identity;
  manualOscFine.Identity().displayAsBipolar = true;
  auto selectManualOscFine = [](auto& module) { return &module.acc.manualOscFine; };
  manualOscFine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualOscFine);
  manualOscFine.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualOscFine);
  manualOscFine.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualOscFine);
  SetManualParamEnabledWhen(manualOscFine, FFGlobalUniTarget::OscFine);

  auto& manualOscPhaseOffset = result->params[(int)FFGlobalUniParam::ManualOscPhaseOffset];
  manualOscPhaseOffset.mode = FBParamMode::VoiceStart;
  manualOscPhaseOffset.defaultText = "0";
  manualOscPhaseOffset.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscPhaseOffset, s); };
  manualOscPhaseOffset.slotFormatterOverrides = true;
  manualOscPhaseOffset.slotCount = FFGlobalUniMaxCount;
  manualOscPhaseOffset.unit = "%";
  manualOscPhaseOffset.id = "{EC98E6C3-0BB3-4E19-A861-5CDDA8C0401F}";
  manualOscPhaseOffset.type = FBParamType::Identity;
  auto selectManualOscPhaseOffset = [](auto& module) { return &module.acc.manualOscPhaseOffset; };
  manualOscPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualOscPhaseOffset);
  manualOscPhaseOffset.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualOscPhaseOffset);
  manualOscPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualOscPhaseOffset);
  SetManualParamEnabledWhen(manualOscPhaseOffset, FFGlobalUniTarget::OscPhaseOffset);

  auto& manualLFORate = result->params[(int)FFGlobalUniParam::ManualLFORate];
  manualLFORate.mode = FBParamMode::Accurate;
  manualLFORate.defaultText = "0";
  manualLFORate.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFORate, s); };
  manualLFORate.slotFormatterOverrides = true;
  manualLFORate.slotCount = FFGlobalUniMaxCount;
  manualLFORate.unit = "%";
  manualLFORate.id = "{A14D92A7-0952-4DF9-98D6-194B1A763DDF}";
  manualLFORate.type = FBParamType::Identity;
  manualLFORate.Identity().displayAsBipolar = true;
  auto selectmanualLFORate = [](auto& module) { return &module.acc.manualLFORate; };
  manualLFORate.scalarAddr = FFSelectScalarParamAddr(selectModule, selectmanualLFORate);
  manualLFORate.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectmanualLFORate);
  manualLFORate.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectmanualLFORate);
  SetManualParamEnabledWhen(manualLFORate, FFGlobalUniTarget::LFORate);

  auto& manualLFOMin = result->params[(int)FFGlobalUniParam::ManualLFOMin];
  manualLFOMin.mode = FBParamMode::Accurate;
  manualLFOMin.defaultText = "0";
  manualLFOMin.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOMin, s); };
  manualLFOMin.slotFormatterOverrides = true;
  manualLFOMin.slotCount = FFGlobalUniMaxCount;
  manualLFOMin.unit = "%";
  manualLFOMin.id = "{BC67CD56-29BF-491D-BA5F-43541B6E506D}";
  manualLFOMin.type = FBParamType::Identity;
  manualLFOMin.Identity().displayAsBipolar = true;
  auto selectManualLFOMin = [](auto& module) { return &module.acc.manualLFOMin; };
  manualLFOMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOMin);
  manualLFOMin.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOMin);
  manualLFOMin.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOMin);
  SetManualParamEnabledWhen(manualLFOMin, FFGlobalUniTarget::LFOMin);

  auto& manualLFOMax = result->params[(int)FFGlobalUniParam::ManualLFOMax];
  manualLFOMax.mode = FBParamMode::Accurate;
  manualLFOMax.defaultText = "0";
  manualLFOMax.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOMax, s); };
  manualLFOMax.slotFormatterOverrides = true;
  manualLFOMax.slotCount = FFGlobalUniMaxCount;
  manualLFOMax.unit = "%";
  manualLFOMax.id = "{D6A4D1B5-8961-4C5B-96CA-792F73227A0C}";
  manualLFOMax.type = FBParamType::Identity;
  manualLFOMax.Identity().displayAsBipolar = true;
  auto selectManualLFOMax = [](auto& module) { return &module.acc.manualLFOMax; };
  manualLFOMax.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOMax);
  manualLFOMax.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOMax);
  manualLFOMax.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOMax);
  SetManualParamEnabledWhen(manualLFOMax, FFGlobalUniTarget::LFOMax);

  auto& manualLFOSkewAX = result->params[(int)FFGlobalUniParam::ManualLFOSkewAX];
  manualLFOSkewAX.mode = FBParamMode::Accurate;
  manualLFOSkewAX.defaultText = "0";
  manualLFOSkewAX.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOSkewAX, s); };
  manualLFOSkewAX.slotFormatterOverrides = true;
  manualLFOSkewAX.slotCount = FFGlobalUniMaxCount;
  manualLFOSkewAX.unit = "%";
  manualLFOSkewAX.id = "{5FABD18F-6F53-4532-AC77-7FC5882B2CD1}";
  manualLFOSkewAX.type = FBParamType::Identity;
  manualLFOSkewAX.Identity().displayAsBipolar = true;
  auto selectManualLFOSkewAX = [](auto& module) { return &module.acc.manualLFOSkewAX; };
  manualLFOSkewAX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOSkewAX);
  manualLFOSkewAX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOSkewAX);
  manualLFOSkewAX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOSkewAX);
  SetManualParamEnabledWhen(manualLFOSkewAX, FFGlobalUniTarget::LFOSkewAX);

  auto& manualLFOSkewAY = result->params[(int)FFGlobalUniParam::ManualLFOSkewAY];
  manualLFOSkewAY.mode = FBParamMode::Accurate;
  manualLFOSkewAY.defaultText = "0";
  manualLFOSkewAY.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOSkewAY, s); };
  manualLFOSkewAY.slotFormatterOverrides = true;
  manualLFOSkewAY.slotCount = FFGlobalUniMaxCount;
  manualLFOSkewAY.unit = "%";
  manualLFOSkewAY.id = "{7C900D23-EC5F-483B-82A3-EB27E2DD9E66}";
  manualLFOSkewAY.type = FBParamType::Identity;
  manualLFOSkewAY.Identity().displayAsBipolar = true;
  auto selectManualLFOSkewAY = [](auto& module) { return &module.acc.manualLFOSkewAY; };
  manualLFOSkewAY.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOSkewAY);
  manualLFOSkewAY.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOSkewAY);
  manualLFOSkewAY.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOSkewAY);
  SetManualParamEnabledWhen(manualLFOSkewAY, FFGlobalUniTarget::LFOSkewAY);

  auto& manualLFOPhaseOffset = result->params[(int)FFGlobalUniParam::ManualLFOPhaseOffset];
  manualLFOPhaseOffset.mode = FBParamMode::VoiceStart;
  manualLFOPhaseOffset.defaultText = "0";
  manualLFOPhaseOffset.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOPhaseOffset, s); };
  manualLFOPhaseOffset.slotFormatterOverrides = true;
  manualLFOPhaseOffset.slotCount = FFGlobalUniMaxCount;
  manualLFOPhaseOffset.unit = "%";
  manualLFOPhaseOffset.id = "{00A68371-5419-4398-8B42-4AADF0955DAF}";
  manualLFOPhaseOffset.type = FBParamType::Identity;
  auto selectManualLFOPhaseOffset = [](auto& module) { return &module.acc.manualLFOPhaseOffset; };
  manualLFOPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOPhaseOffset);
  manualLFOPhaseOffset.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOPhaseOffset);
  manualLFOPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOPhaseOffset);
  SetManualParamEnabledWhen(manualLFOPhaseOffset, FFGlobalUniTarget::LFOPhaseOffset);

  auto& manualVFXParamA = result->params[(int)FFGlobalUniParam::ManualVFXParamA];
  manualVFXParamA.mode = FBParamMode::Accurate;
  manualVFXParamA.defaultText = "0";
  manualVFXParamA.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamA, s); };
  manualVFXParamA.slotFormatterOverrides = true;
  manualVFXParamA.slotCount = FFGlobalUniMaxCount;
  manualVFXParamA.unit = "%";
  manualVFXParamA.id = "{E383FFF3-6549-44A4-878B-9BE81EB4A69F}";
  manualVFXParamA.type = FBParamType::Identity;
  manualVFXParamA.Identity().displayAsBipolar = true;
  auto selectManualVFXParamA = [](auto& module) { return &module.acc.manualVFXParamA; };
  manualVFXParamA.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamA);
  manualVFXParamA.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamA);
  manualVFXParamA.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamA);
  SetManualParamEnabledWhen(manualVFXParamA, FFGlobalUniTarget::VFXParamA);

  auto& manualVFXParamB = result->params[(int)FFGlobalUniParam::ManualVFXParamB];
  manualVFXParamB.mode = FBParamMode::Accurate;
  manualVFXParamB.defaultText = "0";
  manualVFXParamB.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamB, s); };
  manualVFXParamB.slotFormatterOverrides = true;
  manualVFXParamB.slotCount = FFGlobalUniMaxCount;
  manualVFXParamB.unit = "%";
  manualVFXParamB.id = "{82D2D353-D454-496B-BB0E-F7B23D20D698}";
  manualVFXParamB.type = FBParamType::Identity;
  manualVFXParamB.Identity().displayAsBipolar = true;
  auto selectManualVFXParamB = [](auto& module) { return &module.acc.manualVFXParamB; };
  manualVFXParamB.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamB);
  manualVFXParamB.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamB);
  manualVFXParamB.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamB);
  SetManualParamEnabledWhen(manualVFXParamB, FFGlobalUniTarget::VFXParamB);

  auto& manualVFXParamC = result->params[(int)FFGlobalUniParam::ManualVFXParamC];
  manualVFXParamC.mode = FBParamMode::Accurate;
  manualVFXParamC.defaultText = "0";
  manualVFXParamC.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamC, s); };
  manualVFXParamC.slotFormatterOverrides = true;
  manualVFXParamC.slotCount = FFGlobalUniMaxCount;
  manualVFXParamC.unit = "%";
  manualVFXParamC.id = "{55539EFD-3561-4F1F-BDDC-FF0CEC2AA14B}";
  manualVFXParamC.type = FBParamType::Identity;
  manualVFXParamC.Identity().displayAsBipolar = true;
  auto selectManualVFXParamC = [](auto& module) { return &module.acc.manualVFXParamC; };
  manualVFXParamC.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamC);
  manualVFXParamC.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamC);
  manualVFXParamC.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamC);
  SetManualParamEnabledWhen(manualVFXParamC, FFGlobalUniTarget::VFXParamC);

  auto& manualVFXParamD = result->params[(int)FFGlobalUniParam::ManualVFXParamD];
  manualVFXParamD.mode = FBParamMode::Accurate;
  manualVFXParamD.defaultText = "0";
  manualVFXParamD.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamD, s); };
  manualVFXParamD.slotFormatterOverrides = true;
  manualVFXParamD.slotCount = FFGlobalUniMaxCount;
  manualVFXParamD.unit = "%";
  manualVFXParamD.id = "{9346FF6B-068F-4F36-A4CD-68A652CF1A2C}";
  manualVFXParamD.type = FBParamType::Identity;
  manualVFXParamD.Identity().displayAsBipolar = true;
  auto selectManualVFXParamD = [](auto& module) { return &module.acc.manualVFXParamD; };
  manualVFXParamD.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamD);
  manualVFXParamD.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamD);
  manualVFXParamD.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamD);
  SetManualParamEnabledWhen(manualVFXParamD, FFGlobalUniTarget::VFXParamD);

  auto& manualEnvOffset = result->params[(int)FFGlobalUniParam::ManualEnvOffset];
  manualEnvOffset.mode = FBParamMode::VoiceStart;
  manualEnvOffset.defaultText = "0";
  manualEnvOffset.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EnvOffset, s); };
  manualEnvOffset.slotFormatterOverrides = true;
  manualEnvOffset.slotCount = FFGlobalUniMaxCount;
  manualEnvOffset.unit = "Sec";
  manualEnvOffset.id = "{AEDA998E-B0B7-4A78-8C52-F6B809AC5352}";
  manualEnvOffset.type = FBParamType::Linear;
  manualEnvOffset.Linear().min = 0.0f;
  manualEnvOffset.Linear().max = 0.0f;
  manualEnvOffset.Linear().editSkewFactor = 0.5f;
  auto selectManualEnvOffset = [](auto& module) { return &module.acc.manualEnvOffset; };
  manualEnvOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEnvOffset);
  manualEnvOffset.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEnvOffset);
  manualEnvOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEnvOffset);
  SetManualParamEnabledWhen(manualEnvOffset, FFGlobalUniTarget::EnvOffset);

  auto& manualEnvStretch = result->params[(int)FFGlobalUniParam::ManualEnvStretch];
  manualEnvStretch.mode = FBParamMode::VoiceStart;
  manualEnvStretch.defaultText = "0";
  manualEnvStretch.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EnvStretch, s); };
  manualEnvStretch.slotFormatterOverrides = true;
  manualEnvStretch.slotCount = FFGlobalUniMaxCount;
  manualEnvStretch.unit = "%";
  manualEnvStretch.id = "{6155B06D-D928-4EBF-9A52-99EAA3341D26}";
  manualEnvStretch.type = FBParamType::Identity;
  manualEnvStretch.Identity().displayAsBipolar = true;
  auto selectManualEnvStretch = [](auto& module) { return &module.acc.manualEnvStretch; };
  manualEnvStretch.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEnvStretch);
  manualEnvStretch.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEnvStretch);
  manualEnvStretch.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEnvStretch);
  SetManualParamEnabledWhen(manualEnvStretch, FFGlobalUniTarget::EnvStretch);

  auto& manualEchoExtend = result->params[(int)FFGlobalUniParam::ManualEchoExtend];
  manualEchoExtend.mode = FBParamMode::VoiceStart;
  manualEchoExtend.defaultText = "0";
  manualEchoExtend.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoExtend, s); };
  manualEchoExtend.slotFormatterOverrides = true;
  manualEchoExtend.slotCount = FFGlobalUniMaxCount;
  manualEchoExtend.unit = "%";
  manualEchoExtend.id = "{B60645F1-F349-4911-AA11-218E188C0D1F}";
  manualEchoExtend.type = FBParamType::Identity;
  manualEchoExtend.Identity().displayAsBipolar = true;
  auto selectManualEchoExtend = [](auto& module) { return &module.acc.manualEchoExtend; };
  manualEchoExtend.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoExtend);
  manualEchoExtend.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoExtend);
  manualEchoExtend.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoExtend);
  SetManualParamEnabledWhen(manualEchoExtend, FFGlobalUniTarget::EchoExtend);

  auto& manualEchoFade = result->params[(int)FFGlobalUniParam::ManualEchoFade];
  manualEchoFade.mode = FBParamMode::VoiceStart;
  manualEchoFade.defaultText = "0";
  manualEchoFade.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFade, s); };
  manualEchoFade.slotFormatterOverrides = true;
  manualEchoFade.slotCount = FFGlobalUniMaxCount;
  manualEchoFade.unit = "%";
  manualEchoFade.id = "{A9F81A4A-1FB3-418B-86F5-8F49C77934B7}";
  manualEchoFade.type = FBParamType::Identity;
  manualEchoFade.Identity().displayAsBipolar = true;
  auto selectManualEchoFade = [](auto& module) { return &module.acc.manualEchoFade; };
  manualEchoFade.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFade);
  manualEchoFade.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFade);
  manualEchoFade.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFade);
  SetManualParamEnabledWhen(manualEchoFade, FFGlobalUniTarget::EchoFade);

  auto& manualEchoTapLevel = result->params[(int)FFGlobalUniParam::ManualEchoTapLevel];
  manualEchoTapLevel.mode = FBParamMode::Accurate;
  manualEchoTapLevel.defaultText = "0";
  manualEchoTapLevel.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapLevel, s); };
  manualEchoTapLevel.slotFormatterOverrides = true;
  manualEchoTapLevel.slotCount = FFGlobalUniMaxCount;
  manualEchoTapLevel.unit = "%";
  manualEchoTapLevel.id = "{A480C75C-3429-44A1-BAAF-44341E793F80}";
  manualEchoTapLevel.type = FBParamType::Identity;
  manualEchoTapLevel.Identity().displayAsBipolar = true;
  auto selectManualEchoTapLevel = [](auto& module) { return &module.acc.manualEchoTapLevel; };
  manualEchoTapLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapLevel);
  manualEchoTapLevel.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapLevel);
  manualEchoTapLevel.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapLevel);
  SetManualParamEnabledWhen(manualEchoTapLevel, FFGlobalUniTarget::EchoTapLevel);

  auto& manualEchoTapDelay = result->params[(int)FFGlobalUniParam::ManualEchoTapDelay];
  manualEchoTapDelay.mode = FBParamMode::Accurate;
  manualEchoTapDelay.defaultText = "0";
  manualEchoTapDelay.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapDelay, s); };
  manualEchoTapDelay.slotFormatterOverrides = true;
  manualEchoTapDelay.slotCount = FFGlobalUniMaxCount;
  manualEchoTapDelay.unit = "%";
  manualEchoTapDelay.id = "{158E67F4-DD9C-4A2A-93B8-6914B8F9DCED}";
  manualEchoTapDelay.type = FBParamType::Identity;
  manualEchoTapDelay.Identity().displayAsBipolar = true;
  auto selectManualEchoTapDelay = [](auto& module) { return &module.acc.manualEchoTapDelay; };
  manualEchoTapDelay.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapDelay);
  manualEchoTapDelay.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapDelay);
  manualEchoTapDelay.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapDelay);
  SetManualParamEnabledWhen(manualEchoTapDelay, FFGlobalUniTarget::EchoTapDelay);

  auto& manualEchoTapBal = result->params[(int)FFGlobalUniParam::ManualEchoTapBal];
  manualEchoTapBal.mode = FBParamMode::Accurate;
  manualEchoTapBal.defaultText = "0";
  manualEchoTapBal.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapBal, s); };
  manualEchoTapBal.slotFormatterOverrides = true;
  manualEchoTapBal.slotCount = FFGlobalUniMaxCount;
  manualEchoTapBal.unit = "%";
  manualEchoTapBal.id = "{1E8EDB12-2E07-47DE-BB0C-39AC596896E0}";
  manualEchoTapBal.type = FBParamType::Identity;
  manualEchoTapBal.Identity().displayAsBipolar = true;
  auto selectManualEchoTapBal = [](auto& module) { return &module.acc.manualEchoTapBal; };
  manualEchoTapBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapBal);
  manualEchoTapBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapBal);
  manualEchoTapBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapBal);
  SetManualParamEnabledWhen(manualEchoTapBal, FFGlobalUniTarget::EchoTapBal);

  auto& manualEchoFdbkDelay = result->params[(int)FFGlobalUniParam::ManualEchoFdbkDelay];
  manualEchoFdbkDelay.mode = FBParamMode::Accurate;
  manualEchoFdbkDelay.defaultText = "0";
  manualEchoFdbkDelay.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkDelay, s); };
  manualEchoFdbkDelay.slotFormatterOverrides = true;
  manualEchoFdbkDelay.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkDelay.unit = "%";
  manualEchoFdbkDelay.id = "{6D51D872-4BFD-4F06-B202-BD971DB527FA}";
  manualEchoFdbkDelay.type = FBParamType::Identity;
  manualEchoFdbkDelay.Identity().displayAsBipolar = true;
  auto selectManualEchoFdbkDelay = [](auto& module) { return &module.acc.manualEchoFdbkDelay; };
  manualEchoFdbkDelay.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkDelay);
  manualEchoFdbkDelay.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkDelay);
  manualEchoFdbkDelay.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkDelay);
  SetManualParamEnabledWhen(manualEchoFdbkDelay, FFGlobalUniTarget::EchoFdbkDelay);

  auto& manualEchoFdbkMix = result->params[(int)FFGlobalUniParam::ManualEchoFdbkMix];
  manualEchoFdbkMix.mode = FBParamMode::Accurate;
  manualEchoFdbkMix.defaultText = "0";
  manualEchoFdbkMix.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkMix, s); };
  manualEchoFdbkMix.slotFormatterOverrides = true;
  manualEchoFdbkMix.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkMix.unit = "%";
  manualEchoFdbkMix.id = "{41E192B1-909D-41B6-93B5-9343460670A2}";
  manualEchoFdbkMix.type = FBParamType::Identity;
  manualEchoFdbkMix.Identity().displayAsBipolar = true;
  auto selectManualEchoFdbkMix = [](auto& module) { return &module.acc.manualEchoFdbkMix; };
  manualEchoFdbkMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkMix);
  manualEchoFdbkMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkMix);
  manualEchoFdbkMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkMix);
  SetManualParamEnabledWhen(manualEchoFdbkMix, FFGlobalUniTarget::EchoFdbkMix);

  auto& manualEchoFdbkAmt = result->params[(int)FFGlobalUniParam::ManualEchoFdbkAmt];
  manualEchoFdbkAmt.mode = FBParamMode::Accurate;
  manualEchoFdbkAmt.defaultText = "0";
  manualEchoFdbkAmt.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkAmt, s); };
  manualEchoFdbkAmt.slotFormatterOverrides = true;
  manualEchoFdbkAmt.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkAmt.unit = "%";
  manualEchoFdbkAmt.id = "{BF96F4F2-74C7-4BA8-8FE5-72320499849E}";
  manualEchoFdbkAmt.type = FBParamType::Identity;
  manualEchoFdbkAmt.Identity().displayAsBipolar = true;
  auto selectManualEchoFdbkAmt = [](auto& module) { return &module.acc.manualEchoFdbkAmt; };
  manualEchoFdbkAmt.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkAmt);
  manualEchoFdbkAmt.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkAmt);
  manualEchoFdbkAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkAmt);
  SetManualParamEnabledWhen(manualEchoFdbkAmt, FFGlobalUniTarget::EchoFdbkAmt);

  auto& manualEchoReverbMix = result->params[(int)FFGlobalUniParam::ManualEchoReverbMix];
  manualEchoReverbMix.mode = FBParamMode::Accurate;
  manualEchoReverbMix.defaultText = "0";
  manualEchoReverbMix.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbMix, s); };
  manualEchoReverbMix.slotFormatterOverrides = true;
  manualEchoReverbMix.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbMix.unit = "%";
  manualEchoReverbMix.id = "{475A0982-1DEE-4636-B532-0D0FCD816DEC}";
  manualEchoReverbMix.type = FBParamType::Identity;
  manualEchoReverbMix.Identity().displayAsBipolar = true;
  auto selectManualEchoReverbMix = [](auto& module) { return &module.acc.manualEchoReverbMix; };
  manualEchoReverbMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbMix);
  manualEchoReverbMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbMix);
  manualEchoReverbMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbMix);
  SetManualParamEnabledWhen(manualEchoReverbMix, FFGlobalUniTarget::EchoReverbMix);

  auto& manualEchoReverbSize = result->params[(int)FFGlobalUniParam::ManualEchoReverbSize];
  manualEchoReverbSize.mode = FBParamMode::Accurate;
  manualEchoReverbSize.defaultText = "0";
  manualEchoReverbSize.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbSize, s); };
  manualEchoReverbSize.slotFormatterOverrides = true;
  manualEchoReverbSize.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbSize.unit = "%";
  manualEchoReverbSize.id = "{8E997A4E-BCF6-4599-9329-377E81F77B5F}";
  manualEchoReverbSize.type = FBParamType::Identity;
  manualEchoReverbSize.Identity().displayAsBipolar = true;
  auto selectManualEchoReverbSize = [](auto& module) { return &module.acc.manualEchoReverbSize; };
  manualEchoReverbSize.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbSize);
  manualEchoReverbSize.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbSize);
  manualEchoReverbSize.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbSize);
  SetManualParamEnabledWhen(manualEchoReverbSize, FFGlobalUniTarget::EchoReverbSize);

  auto& manualEchoReverbDamp = result->params[(int)FFGlobalUniParam::ManualEchoReverbDamp];
  manualEchoReverbDamp.mode = FBParamMode::Accurate;
  manualEchoReverbDamp.defaultText = "0";
  manualEchoReverbDamp.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbDamp, s); };
  manualEchoReverbDamp.slotFormatterOverrides = true;
  manualEchoReverbDamp.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbDamp.unit = "%";
  manualEchoReverbDamp.id = "{6E27DACE-DEA9-4168-8752-6BFB0B09002E}";
  manualEchoReverbDamp.type = FBParamType::Identity;
  manualEchoReverbDamp.Identity().displayAsBipolar = true;
  auto selectManualEchoReverbDamp = [](auto& module) { return &module.acc.manualEchoReverbDamp; };
  manualEchoReverbDamp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbDamp);
  manualEchoReverbDamp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbDamp);
  manualEchoReverbDamp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbDamp);
  SetManualParamEnabledWhen(manualEchoReverbDamp, FFGlobalUniTarget::EchoReverbDamp);

  return result;
}