#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGraph.hpp>

#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

float
FFGlobalUniTargetGetDefaultValue(
  FFGlobalUniTarget target)
{
  switch (target)
  {
  case FFGlobalUniTarget::VMixAmp:
  case FFGlobalUniTarget::OscGain:
    return 1.0f;
  case FFGlobalUniTarget::LFOPhaseOffset:
  case FFGlobalUniTarget::OscPhaseOffset:
    return 0.0f;
  default:
    return 0.5f;
  }
}

FFGlobalUniTarget
FFParamToGlobalUniTarget(FBRuntimeTopo const* topo, int index)
{
  auto const& indices = topo->audio.params[index].topoIndices;
  if (indices.module.index == (int)FFModuleType::VoiceModule && indices.param.index == (int)FFVoiceModuleParam::Coarse)
    return FFGlobalUniTarget::VoiceCoarse;
  if (indices.module.index == (int)FFModuleType::VoiceModule && indices.param.index == (int)FFVoiceModuleParam::Fine)
    return FFGlobalUniTarget::VoiceFine;
  if (indices.module.index == (int)FFModuleType::VMix && indices.param.index == (int)FFVMixParam::Amp)
    return FFGlobalUniTarget::VMixAmp;
  if (indices.module.index == (int)FFModuleType::VMix && indices.param.index == (int)FFVMixParam::Bal)
    return FFGlobalUniTarget::VMixBal;
  if (indices.module.index == (int)FFModuleType::Osci && indices.param.index == (int)FFOsciParam::Gain)
    return FFGlobalUniTarget::OscGain;
  if (indices.module.index == (int)FFModuleType::Osci && indices.param.index == (int)FFOsciParam::Pan)
    return FFGlobalUniTarget::OscPan;
  if (indices.module.index == (int)FFModuleType::Osci && indices.param.index == (int)FFOsciParam::Coarse)
    return FFGlobalUniTarget::OscCoarse;
  if (indices.module.index == (int)FFModuleType::Osci && indices.param.index == (int)FFOsciParam::Fine)
    return FFGlobalUniTarget::OscFine;
  if (indices.module.index == (int)FFModuleType::Osci && indices.param.index == (int)FFOsciParam::Phase)
    return FFGlobalUniTarget::OscPhaseOffset;
  if (indices.module.index == (int)FFModuleType::VLFO && indices.param.index == (int)FFLFOParam::RateHz)
    return FFGlobalUniTarget::LFORate;
  if (indices.module.index == (int)FFModuleType::VLFO && indices.param.index == (int)FFLFOParam::Min)
    return FFGlobalUniTarget::LFOMin;
  if (indices.module.index == (int)FFModuleType::VLFO && indices.param.index == (int)FFLFOParam::Max)
    return FFGlobalUniTarget::LFOMax;
  if (indices.module.index == (int)FFModuleType::VLFO && indices.param.index == (int)FFLFOParam::Phase)
    return FFGlobalUniTarget::LFOPhaseOffset;
  if (indices.module.index == (int)FFModuleType::VLFO && indices.param.index == (int)FFLFOParam::SkewAXAmt)
    return FFGlobalUniTarget::LFOSkewAX;
  if (indices.module.index == (int)FFModuleType::VLFO && indices.param.index == (int)FFLFOParam::SkewAYAmt)
    return FFGlobalUniTarget::LFOSkewAY;
  if (indices.module.index == (int)FFModuleType::Env && indices.param.index == (int)FFEnvParam::StageSlope)
    return FFGlobalUniTarget::EnvSlope;
  if (indices.module.index == (int)FFModuleType::Env && indices.param.index == (int)FFEnvParam::StageTime)
    return FFGlobalUniTarget::EnvStretch;
  if (indices.module.index == (int)FFModuleType::VEffect && indices.param.index == (int)FFEffectParam::StVarFreqFreq)
    return (FFGlobalUniTarget)((int)FFGlobalUniTarget::VFXParamA + indices.param.slot);
  if (indices.module.index == (int)FFModuleType::VEffect && indices.param.index == (int)FFEffectParam::StVarPitchCoarse)
    return (FFGlobalUniTarget)((int)FFGlobalUniTarget::VFXParamA + indices.param.slot);
  if (indices.module.index == (int)FFModuleType::VEffect && indices.param.index == (int)FFEffectParam::CombFreqFreqMin)
    return (FFGlobalUniTarget)((int)FFGlobalUniTarget::VFXParamA + indices.param.slot);
  if (indices.module.index == (int)FFModuleType::VEffect && indices.param.index == (int)FFEffectParam::CombFreqFreqPlus)
    return (FFGlobalUniTarget)((int)FFGlobalUniTarget::VFXParamA + indices.param.slot);
  if (indices.module.index == (int)FFModuleType::VEffect && indices.param.index == (int)FFEffectParam::CombPitchCoarseMin)
    return (FFGlobalUniTarget)((int)FFGlobalUniTarget::VFXParamA + indices.param.slot);
  if (indices.module.index == (int)FFModuleType::VEffect && indices.param.index == (int)FFEffectParam::CombPitchCoarsePlus)
    return (FFGlobalUniTarget)((int)FFGlobalUniTarget::VFXParamA + indices.param.slot);
  if (indices.module.index == (int)FFModuleType::VEffect && indices.param.index == (int)FFEffectParam::DistDrive)
    return (FFGlobalUniTarget)((int)FFGlobalUniTarget::VFXParamA + indices.param.slot);
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::VoiceExtendTime)
    return FFGlobalUniTarget::EchoExtend;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::VoiceFadeTime)
    return FFGlobalUniTarget::EchoFade;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::TapsMix)
    return FFGlobalUniTarget::EchoTapsMix;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::TapBalance)
    return FFGlobalUniTarget::EchoTapBal;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::TapDelayTime)
    return FFGlobalUniTarget::EchoTapDelay;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::TapLPFreq)
    return FFGlobalUniTarget::EchoTapLPF;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::TapHPFreq)
    return FFGlobalUniTarget::EchoTapHPF;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::FeedbackDelayTime)
    return FFGlobalUniTarget::EchoFdbkDelay;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::FeedbackMix)
    return FFGlobalUniTarget::EchoFdbkMix;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::FeedbackAmount)
    return FFGlobalUniTarget::EchoFdbkAmt;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::FeedbackLPFreq)
    return FFGlobalUniTarget::EchoFdbkLPF;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::FeedbackHPFreq)
    return FFGlobalUniTarget::EchoFdbkHPF;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::ReverbMix)
    return FFGlobalUniTarget::EchoReverbMix;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::ReverbSize)
    return FFGlobalUniTarget::EchoReverbSize;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::ReverbDamp)
    return FFGlobalUniTarget::EchoReverbDamp;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::ReverbLPFreq)
    return FFGlobalUniTarget::EchoReverbLPF;
  if (indices.module.index == (int)FFModuleType::VEcho && indices.param.index == (int)FFEchoParam::ReverbHPFreq)
    return FFGlobalUniTarget::EchoReverbHPF;
  return (FFGlobalUniTarget)-1;
}

FFModuleType
FFGlobalUniTargetToModule(FFGlobalUniTarget target)
{
  switch (target)
  {
  case FFGlobalUniTarget::VoiceCoarse: 
  case FFGlobalUniTarget::VoiceFine: return FFModuleType::VoiceModule;
  case FFGlobalUniTarget::VMixAmp: 
  case FFGlobalUniTarget::VMixBal: return FFModuleType::VMix;
  case FFGlobalUniTarget::OscGain: 
  case FFGlobalUniTarget::OscPan: 
  case FFGlobalUniTarget::OscCoarse: 
  case FFGlobalUniTarget::OscFine: 
  case FFGlobalUniTarget::OscPhaseOffset: return FFModuleType::Osci;
  case FFGlobalUniTarget::LFORate: 
  case FFGlobalUniTarget::LFOMin: 
  case FFGlobalUniTarget::LFOMax:
  case FFGlobalUniTarget::LFOSkewAX: 
  case FFGlobalUniTarget::LFOSkewAY: 
  case FFGlobalUniTarget::LFOPhaseOffset: return FFModuleType::VLFO;
  case FFGlobalUniTarget::EnvSlope: 
  case FFGlobalUniTarget::EnvStretch: return FFModuleType::Env;
  case FFGlobalUniTarget::VFXParamA: 
  case FFGlobalUniTarget::VFXParamB: 
  case FFGlobalUniTarget::VFXParamC: 
  case FFGlobalUniTarget::VFXParamD: return FFModuleType::VEffect;
  case FFGlobalUniTarget::EchoExtend: 
  case FFGlobalUniTarget::EchoFade: 
  case FFGlobalUniTarget::EchoTapsMix:
  case FFGlobalUniTarget::EchoTapDelay:
  case FFGlobalUniTarget::EchoTapBal: 
  case FFGlobalUniTarget::EchoTapLPF:
  case FFGlobalUniTarget::EchoTapHPF:
  case FFGlobalUniTarget::EchoFdbkDelay: 
  case FFGlobalUniTarget::EchoFdbkMix: 
  case FFGlobalUniTarget::EchoFdbkAmt: 
  case FFGlobalUniTarget::EchoFdbkLPF:
  case FFGlobalUniTarget::EchoFdbkHPF:
  case FFGlobalUniTarget::EchoReverbMix: 
  case FFGlobalUniTarget::EchoReverbSize: 
  case FFGlobalUniTarget::EchoReverbDamp: 
  case FFGlobalUniTarget::EchoReverbLPF:
  case FFGlobalUniTarget::EchoReverbHPF: return FFModuleType::VEcho;
  default: FB_ASSERT(false); return (FFModuleType)-1;
  }
}

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
  case FFGlobalUniTarget::EnvSlope: return "Env Slope";
  case FFGlobalUniTarget::EnvStretch: return "Env Stretch";
  case FFGlobalUniTarget::VFXParamA: return "VFX Param A";
  case FFGlobalUniTarget::VFXParamB: return "VFX Param B";
  case FFGlobalUniTarget::VFXParamC: return "VFX Param C";
  case FFGlobalUniTarget::VFXParamD: return "VFX Param D";
  case FFGlobalUniTarget::EchoExtend: return "VEcho Extend";
  case FFGlobalUniTarget::EchoFade: return "VEcho Fade";
  case FFGlobalUniTarget::EchoTapsMix: return "VEcho Taps Mix";
  case FFGlobalUniTarget::EchoTapDelay: return "VEcho Tap Dly";
  case FFGlobalUniTarget::EchoTapBal: return "VEcho Tap Bal";
  case FFGlobalUniTarget::EchoTapLPF: return "VEcho Tap LPF";
  case FFGlobalUniTarget::EchoTapHPF: return "VEcho Tap HPF";
  case FFGlobalUniTarget::EchoFdbkDelay: return "VEcho Fdbk Dly";
  case FFGlobalUniTarget::EchoFdbkMix: return "VEcho Fdbk Mix";
  case FFGlobalUniTarget::EchoFdbkAmt: return "VEcho Fdbk Amt";
  case FFGlobalUniTarget::EchoFdbkLPF: return "VEcho Fdbk LPF";
  case FFGlobalUniTarget::EchoFdbkHPF: return "VEcho Fdbk HPF";
  case FFGlobalUniTarget::EchoReverbMix: return "VEcho Rvrb Mix";
  case FFGlobalUniTarget::EchoReverbSize: return "VEcho Rvrb Size";
  case FFGlobalUniTarget::EchoReverbDamp: return "VEcho Rvrb Damp";
  case FFGlobalUniTarget::EchoReverbLPF: return "VEcho Rvrb LPF";
  case FFGlobalUniTarget::EchoReverbHPF: return "VEcho Rvrb HPF";
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
    { (int)FFGlobalUniParam::Mode, (int)target },
    { (int)FFGlobalUniParam::OpType, (int)target }
  }, [](auto const& slots, auto const& vs) { 
      return slots[1] < vs[0] && vs[2] == (int)FFGlobalUniMode::Manual && vs[3] != 0; 
  });
}

std::unique_ptr<FBStaticModule>
FFMakeGlobalUniTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Unison";
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
  voiceCount.description = "Unison Voice Count";
  voiceCount.type = FBParamType::Discrete;
  voiceCount.Discrete().valueCount = FFGlobalUniMaxCount;
  voiceCount.Discrete().valueOffset = 1;
  auto selectVoiceCount = [](auto& module) { return &module.block.voiceCount; };
  voiceCount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoiceCount);
  voiceCount.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectVoiceCount);
  voiceCount.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoiceCount);

  auto& mode = result->params[(int)FFGlobalUniParam::Mode];
  mode.mode = FBParamMode::Block;
  mode.name = "Mode";
  mode.display = "Mode";
  mode.defaultTextSelector = [](int, int, int ps) {
    return ps == (int)FFGlobalUniTarget::VoiceFine || ps == (int)FFGlobalUniTarget::OscPhaseOffset ? "Auto Lin" : "Off"; };
  mode.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Mode"; };
  mode.slotFormatterOverrides = true;
  mode.slotCount = (int)FFGlobalUniTarget::Count;
  mode.id = "{B5809A8A-B0A9-40B2-8A0B-413121869836}";
  mode.description = "Unison Param Mode";
  mode.type = FBParamType::List;
  mode.List().items = {
    { "{328055DD-795F-402B-9B16-F30589866295}", "Off" },
    { "{7645E42A-1249-4483-9019-4F92AD9D0FF7}", "Manual" },
    { "{A7C3D7CD-2C1E-4691-831B-2688B751335C}", "Auto Lin" },
    { "{770E5F05-0041-4750-805F-BF08A5135B1B}", "Auto Exp" } };
  auto selectMode = [](auto& module) { return &module.block.mode; };
  mode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectMode);
  mode.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectMode);
  mode.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectMode);
  mode.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::VoiceCount } }, [](auto const& vs) { return vs[0] > 1; });

  auto& opType = result->params[(int)FFGlobalUniParam::OpType];
  opType.mode = FBParamMode::Block;
  opType.name = "Op";
  opType.display = "Op";
  opType.slotCount = (int)FFGlobalUniTarget::Count;
  opType.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Op"; };
  opType.slotFormatterOverrides = true;
  opType.id = "{84EAEA86-D75E-403F-8EEC-231F02F4E053}";
  opType.description = "Unison Param Operator";
  opType.defaultTextSelector = [](int /*mi*/, int, int ps) { 
    auto target = (FFGlobalUniTarget)ps;
    switch (target)
    {
    case FFGlobalUniTarget::VoiceCoarse: return "BP Add BP";
    case FFGlobalUniTarget::VMixAmp: return "UP Remap";
    case FFGlobalUniTarget::OscGain: return "UP Remap";
    case FFGlobalUniTarget::OscCoarse: return "BP Add BP";
    case FFGlobalUniTarget::OscPhaseOffset: return "Phs Wrap";
    case FFGlobalUniTarget::LFOPhaseOffset: return "Phs Wrap";
    case FFGlobalUniTarget::LFORate: return "BP Add UP";
    case FFGlobalUniTarget::EnvStretch: return "BP Stack";
    case FFGlobalUniTarget::EchoFdbkDelay: return "BP Add UP";
    case FFGlobalUniTarget::EchoTapDelay: return "BP Add UP";
    default: return "BP Rmp";
    }
  };
  opType.type = FBParamType::List;
  opType.List().items = {
    { "{A1E456A1-05D9-4915-8C90-0076FFD9DADF}", FFModulationOpTypeToString(FFModulationOpType::Off) },
    { "{68818E5D-62D3-433A-A81A-7FAA7EA11018}", FFModulationOpTypeToString(FFModulationOpType::UPAddU) },
    { "{F45E048F-41C0-43CA-875A-8CE67A6D4815}", FFModulationOpTypeToString(FFModulationOpType::UPAddB) },
    { "{AD641260-F205-497E-B483-330CFA025378}", FFModulationOpTypeToString(FFModulationOpType::UPMul) },
    { "{5D97E841-675B-423F-B30C-06AD60AC0A54}", FFModulationOpTypeToString(FFModulationOpType::UPStack) },
    { "{01014CF0-B70C-4E60-9A78-FF560CF05ECF}", FFModulationOpTypeToString(FFModulationOpType::UPRemap) },
    { "{C18F6A70-944C-4A9B-8A01-561E1B6B93D4}", FFModulationOpTypeToString(FFModulationOpType::BPAddB) },
    { "{4464FDBF-1CDA-4023-85CC-D3A90CC4F47B}", FFModulationOpTypeToString(FFModulationOpType::BPAddU) },
    { "{3130BBE8-D204-450D-A3D2-AC4266FB8E4B}", FFModulationOpTypeToString(FFModulationOpType::BPMul) },
    { "{69D5AD4B-BD0B-42A6-A252-A0A43D425F89}", FFModulationOpTypeToString(FFModulationOpType::BPStack) },
    { "{D96CFB4F-1F3C-434C-A78D-BA624B3DFA10}", FFModulationOpTypeToString(FFModulationOpType::BPRemap) },
    { "{79662DE0-C38D-4ECD-BCC1-5CDD210E0015}", FFModulationOpTypeToString(FFModulationOpType::PhaseWrap) } };
  auto selectOpType = [](auto& module) { return &module.block.opType; };
  opType.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOpType);
  opType.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOpType);
  opType.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOpType);
  opType.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::VoiceCount, (int)FFGlobalUniParam::Mode } }, [](auto const& vs) { return vs[0] > 1 && vs[1] != 0; });

  auto& autoSpread = result->params[(int)FFGlobalUniParam::AutoSpread];
  autoSpread.mode = FBParamMode::Accurate;
  autoSpread.name = "Auto Sprd";
  autoSpread.display = "Sprd";
  autoSpread.defaultTextSelector = [](int, int, int ps) {
    if (ps == (int)FFGlobalUniTarget::VoiceFine || ps == (int)FFGlobalUniTarget::OscFine)
      return "16.7";
    if (ps == (int)FFGlobalUniTarget::OscPhaseOffset || ps == (int)FFGlobalUniTarget::LFOPhaseOffset)
      return "50";
    if (ps == (int)FFGlobalUniTarget::VMixAmp || ps == (int)FFGlobalUniTarget::OscGain)
      return "100";
    return "0";
  };
  autoSpread.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Spread"; };
  autoSpread.slotFormatterOverrides = true;
  autoSpread.slotCount = (int)FFGlobalUniTarget::Count;
  autoSpread.id = "{6048E64F-F600-49AF-9DE6-16FB77134A40}";
  autoSpread.description = "Unison Auto Param Spread";
  autoSpread.unit = "%";
  autoSpread.type = FBParamType::Identity;
  auto selectAutoSpread = [](auto& module) { return &module.acc.autoSpread; };
  autoSpread.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoSpread);
  autoSpread.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAutoSpread);
  autoSpread.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoSpread);
  autoSpread.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode, (int)FFGlobalUniParam::VoiceCount, (int)FFGlobalUniParam::OpType } }, [](auto const& vs) { return (vs[0] == (int)FFGlobalUniMode::AutoLinear || vs[0] == (int)FFGlobalUniMode::AutoExp) && vs[1] > 1 && vs[2] != 0; });

  auto& autoSkew = result->params[(int)FFGlobalUniParam::AutoSkew];
  autoSkew.mode = FBParamMode::Accurate;
  autoSkew.defaultText = "50";
  autoSkew.name = "Auto Skew";
  autoSkew.display = "Skew";
  autoSkew.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Skew"; };
  autoSkew.slotFormatterOverrides = true;
  autoSkew.slotCount = (int)FFGlobalUniTarget::Count;
  autoSkew.id = "{F406EEC6-0A27-4486-BDE3-42947993DFE8}";
  autoSkew.description = "Unison Auto Param Skew";
  autoSkew.unit = "%";
  autoSkew.type = FBParamType::Identity;
  auto selectAutoSkew = [](auto& module) { return &module.acc.autoSkew; };
  autoSkew.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoSkew);
  autoSkew.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAutoSkew);
  autoSkew.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoSkew);
  // Only enabled for 4+ voices, as near/far/center are fixed.
  autoSkew.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode, (int)FFGlobalUniParam::VoiceCount, (int)FFGlobalUniParam::OpType } }, [](auto const& vs) { return vs[0] == (int)FFGlobalUniMode::AutoExp && vs[1] > 3 && vs[2] != 0; });

  auto& autoRand = result->params[(int)FFGlobalUniParam::AutoRand];
  autoRand.mode = FBParamMode::VoiceStart;
  autoRand.display = "Rnd";
  autoRand.name = "Audo Rand Amt";
  autoRand.defaultTextSelector = [](int, int, int ps) {
    if (ps == (int)FFGlobalUniTarget::OscPhaseOffset)
      return "50";
    return "0";
  };
  autoRand.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Rand"; };
  autoRand.slotFormatterOverrides = true;
  autoRand.slotCount = (int)FFGlobalUniTarget::Count;
  autoRand.id = "{74DDA592-503E-4A8E-A026-EB15A3977D7B}";
  autoRand.description = "Unison Auto Param Random Amount";
  autoRand.unit = "%";
  autoRand.type = FBParamType::Identity;
  auto selectAutoRand = [](auto& module) { return &module.acc.autoRand; };
  autoRand.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoRand);
  autoRand.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAutoRand);
  autoRand.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoRand);
  autoRand.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode, (int)FFGlobalUniParam::VoiceCount, (int)FFGlobalUniParam::OpType } }, [](auto const& vs) { return (vs[0] == (int)FFGlobalUniMode::AutoLinear || vs[0] == (int)FFGlobalUniMode::AutoExp) && vs[1] > 1 && vs[2] != 0; });

  auto& autoRandSeed = result->params[(int)FFGlobalUniParam::AutoRandSeed];
  autoRandSeed.mode = FBParamMode::Block;
  autoRandSeed.defaultText = "0";
  autoRandSeed.display = "Seed";
  autoRandSeed.name = "Auto Rand Seed";
  autoRandSeed.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Seed"; };
  autoRandSeed.slotFormatterOverrides = true;
  autoRandSeed.slotCount = (int)FFGlobalUniTarget::Count;
  autoRandSeed.id = "{3D9A110B-B3F4-4700-B1A6-D3F5F7CEA368}";
  autoRandSeed.description = "Unison Auto Param Random Seed";
  autoRandSeed.type = FBParamType::Discrete;
  autoRandSeed.Discrete().valueCount = FFGlobalUniMaxSeed + 1;
  auto selectAutoRandSeed = [](auto& module) { return &module.block.autoRandSeed; };
  autoRandSeed.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoRandSeed);
  autoRandSeed.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectAutoRandSeed);
  autoRandSeed.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoRandSeed);
  autoRandSeed.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode, (int)FFGlobalUniParam::VoiceCount,  (int)FFGlobalUniParam::OpType } }, [](auto const& vs) { return (vs[0] == (int)FFGlobalUniMode::AutoLinear || vs[0] == (int)FFGlobalUniMode::AutoExp) && vs[1] > 1 && vs[2] != 0; });

  auto& autoRandFree = result->params[(int)FFGlobalUniParam::AutoRandFree];
  autoRandFree.mode = FBParamMode::Block;
  autoRandFree.defaultText = "Off";
  autoRandFree.name = "Auto Rand Free";
  autoRandFree.display = "Free";
  autoRandFree.slotFormatter = [](auto const&, auto, int s) { return FFGlobalUniTargetToString((FFGlobalUniTarget)s) + " Free"; };
  autoRandFree.slotFormatterOverrides = true;
  autoRandFree.slotCount = (int)FFGlobalUniTarget::Count;
  autoRandFree.id = "{8ED21529-ABEF-4D1C-BA27-768B0016BEC9}";
  autoRandFree.description = "Unison Auto Param Random Free-Running";
  autoRandFree.type = FBParamType::Boolean;
  auto selectAutoRandFree = [](auto& module) { return &module.block.autoRandFree; };
  autoRandFree.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAutoRandFree);
  autoRandFree.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectAutoRandFree);
  autoRandFree.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAutoRandFree);
  autoRandFree.dependencies.enabled.audio.WhenSimple({ { (int)FFGlobalUniParam::Mode, (int)FFGlobalUniParam::VoiceCount, (int)FFGlobalUniParam::OpType } }, [](auto const& vs) { return (vs[0] == (int)FFGlobalUniMode::AutoLinear || vs[0] == (int)FFGlobalUniMode::AutoExp) && vs[1] > 1 && vs[2] != 0; });

  auto& manualVoiceCoarse = result->params[(int)FFGlobalUniParam::ManualVoiceCoarse];
  manualVoiceCoarse.mode = FBParamMode::Accurate;
  manualVoiceCoarse.defaultText = "0";
  manualVoiceCoarse.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VoiceCoarse, s); };
  manualVoiceCoarse.slotFormatterOverrides = true;
  manualVoiceCoarse.slotCount = FFGlobalUniMaxCount;
  manualVoiceCoarse.id = "{0D24BD6A-DE67-4CE6-8272-BC290D5D7D2D}";
  manualVoiceCoarse.description = "Unison Manual Param Voice Coarse Pitch";
  manualVoiceCoarse.type = FBParamType::Linear;
  manualVoiceCoarse.Linear().min = -FFModCoarseSemis;
  manualVoiceCoarse.Linear().max = FFModCoarseSemis;
  auto selectManualVoiceCoarse = [](auto& module) { return &module.acc.manualVoiceCoarse; };
  manualVoiceCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVoiceCoarse);
  manualVoiceCoarse.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVoiceCoarse);
  manualVoiceCoarse.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVoiceCoarse);
  SetManualParamEnabledWhen(manualVoiceCoarse, FFGlobalUniTarget::VoiceCoarse);

  auto& manualVoiceFine = result->params[(int)FFGlobalUniParam::ManualVoiceFine];
  manualVoiceFine.mode = FBParamMode::Accurate;
  manualVoiceFine.defaultText = "50";
  manualVoiceFine.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VoiceFine, s); };
  manualVoiceFine.slotFormatterOverrides = true;
  manualVoiceFine.slotCount = FFGlobalUniMaxCount;
  manualVoiceFine.id = "{4A3335C3-9239-4D90-A53A-F73807FEC0B7}";
  manualVoiceFine.description = "Unison Manual Param Voice Fine Pitch";
  manualVoiceFine.unit = "%";
  manualVoiceFine.type = FBParamType::Identity;
  auto selectManualVoiceFine = [](auto& module) { return &module.acc.manualVoiceFine; };
  manualVoiceFine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVoiceFine);
  manualVoiceFine.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVoiceFine);
  manualVoiceFine.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVoiceFine);
  SetManualParamEnabledWhen(manualVoiceFine, FFGlobalUniTarget::VoiceFine);

  auto& manualVMixAmp = result->params[(int)FFGlobalUniParam::ManualVMixAmp];
  manualVMixAmp.mode = FBParamMode::Accurate;
  manualVMixAmp.defaultText = "50";
  manualVMixAmp.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VMixAmp, s); };
  manualVMixAmp.slotFormatterOverrides = true;
  manualVMixAmp.slotCount = FFGlobalUniMaxCount;
  manualVMixAmp.id = "{CA16FDA4-CB83-46F4-B236-5382DAA1CFD6}";
  manualVMixAmp.description = "Unison Manual Param VMix Amp";
  manualVMixAmp.unit = "%";
  manualVMixAmp.type = FBParamType::Identity;
  auto selectManualVMixAmp = [](auto& module) { return &module.acc.manualVMixAmp; };
  manualVMixAmp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVMixAmp);
  manualVMixAmp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVMixAmp);
  manualVMixAmp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVMixAmp);
  SetManualParamEnabledWhen(manualVMixAmp, FFGlobalUniTarget::VMixAmp);

  auto& manualVMixBal = result->params[(int)FFGlobalUniParam::ManualVMixBal];
  manualVMixBal.mode = FBParamMode::Accurate;
  manualVMixBal.defaultText = "50";
  manualVMixBal.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VMixBal, s); };
  manualVMixBal.slotFormatterOverrides = true;
  manualVMixBal.slotCount = FFGlobalUniMaxCount;
  manualVMixBal.id = "{A6AD53AB-3FBD-4579-9B2C-D97118B16E70}";
  manualVMixBal.description = "Unison Manual Param VMix Stereo Balance";
  manualVMixBal.unit = "%";
  manualVMixBal.type = FBParamType::Identity;
  auto selectManualVMixBal = [](auto& module) { return &module.acc.manualVMixBal; };
  manualVMixBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVMixBal);
  manualVMixBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVMixBal);
  manualVMixBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVMixBal);
  SetManualParamEnabledWhen(manualVMixBal, FFGlobalUniTarget::VMixBal);

  auto& manualOscGain = result->params[(int)FFGlobalUniParam::ManualOscGain];
  manualOscGain.mode = FBParamMode::Accurate;
  manualOscGain.defaultText = "50";
  manualOscGain.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscGain, s); };
  manualOscGain.slotFormatterOverrides = true;
  manualOscGain.slotCount = FFGlobalUniMaxCount;
  manualOscGain.unit = "%";
  manualOscGain.id = "{847E5FD5-8DAC-43B1-BA0E-125FD916844C}";
  manualOscGain.description = "Unison Manual Param Osc Gain";
  manualOscGain.type = FBParamType::Identity;
  auto selectManualOscGain = [](auto& module) { return &module.acc.manualOscGain; };
  manualOscGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualOscGain);
  manualOscGain.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualOscGain);
  manualOscGain.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualOscGain);
  SetManualParamEnabledWhen(manualOscGain, FFGlobalUniTarget::OscGain);

  auto& manualOscPan = result->params[(int)FFGlobalUniParam::ManualOscPan];
  manualOscPan.mode = FBParamMode::Accurate;
  manualOscPan.defaultText = "50";
  manualOscPan.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscPan, s); };
  manualOscPan.slotFormatterOverrides = true;
  manualOscPan.slotCount = FFGlobalUniMaxCount;
  manualOscPan.unit = "%";
  manualOscPan.id = "{9CACBCF5-9D85-431B-986B-E2B50DC3336A}";
  manualOscPan.description = "Unison Manual Param Osc Pan";
  manualOscPan.type = FBParamType::Identity;
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
  manualOscCoarse.id = "{C16C50D5-00C0-4393-87F9-1A7C457F4483}";
  manualOscCoarse.description = "Unison Manual Param Osc Coarse Pitch";
  manualOscCoarse.type = FBParamType::Linear;
  manualOscCoarse.Linear().min = -FFModCoarseSemis;
  manualOscCoarse.Linear().max = FFModCoarseSemis;
  auto selectmanualOscCoarse = [](auto& module) { return &module.acc.manualOscCoarse; };
  manualOscCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectmanualOscCoarse);
  manualOscCoarse.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectmanualOscCoarse);
  manualOscCoarse.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectmanualOscCoarse);
  SetManualParamEnabledWhen(manualOscCoarse, FFGlobalUniTarget::OscCoarse);

  auto& manualOscFine = result->params[(int)FFGlobalUniParam::ManualOscFine];
  manualOscFine.mode = FBParamMode::Accurate;
  manualOscFine.defaultText = "50";
  manualOscFine.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::OscFine, s); };
  manualOscFine.slotFormatterOverrides = true;
  manualOscFine.slotCount = FFGlobalUniMaxCount;
  manualOscFine.id = "{D32C05E9-BE89-43CB-887F-AF7B1001518F}";
  manualOscFine.description = "Unison Manual Param Osc Fine Pitch";
  manualOscFine.unit = "%";
  manualOscFine.type = FBParamType::Identity;
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
  manualOscPhaseOffset.description = "Unison Manual Param Osc Phase Offset";
  manualOscPhaseOffset.type = FBParamType::Identity;
  auto selectManualOscPhaseOffset = [](auto& module) { return &module.acc.manualOscPhaseOffset; };
  manualOscPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualOscPhaseOffset);
  manualOscPhaseOffset.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualOscPhaseOffset);
  manualOscPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualOscPhaseOffset);
  SetManualParamEnabledWhen(manualOscPhaseOffset, FFGlobalUniTarget::OscPhaseOffset);

  auto& manualLFORate = result->params[(int)FFGlobalUniParam::ManualLFORate];
  manualLFORate.mode = FBParamMode::Accurate;
  manualLFORate.defaultText = "50";
  manualLFORate.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFORate, s); };
  manualLFORate.slotFormatterOverrides = true;
  manualLFORate.slotCount = FFGlobalUniMaxCount;
  manualLFORate.unit = "%";
  manualLFORate.id = "{A14D92A7-0952-4DF9-98D6-194B1A763DDF}";
  manualLFORate.description = "Unison Manual Param LFO Rate";
  manualLFORate.type = FBParamType::Identity;
  auto selectmanualLFORate = [](auto& module) { return &module.acc.manualLFORate; };
  manualLFORate.scalarAddr = FFSelectScalarParamAddr(selectModule, selectmanualLFORate);
  manualLFORate.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectmanualLFORate);
  manualLFORate.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectmanualLFORate);
  SetManualParamEnabledWhen(manualLFORate, FFGlobalUniTarget::LFORate);

  auto& manualLFOMin = result->params[(int)FFGlobalUniParam::ManualLFOMin];
  manualLFOMin.mode = FBParamMode::Accurate;
  manualLFOMin.defaultText = "50";
  manualLFOMin.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOMin, s); };
  manualLFOMin.slotFormatterOverrides = true;
  manualLFOMin.slotCount = FFGlobalUniMaxCount;
  manualLFOMin.unit = "%";
  manualLFOMin.id = "{BC67CD56-29BF-491D-BA5F-43541B6E506D}";
  manualLFOMin.description = "Unison Manual Param LFO Min";
  manualLFOMin.type = FBParamType::Identity;
  auto selectManualLFOMin = [](auto& module) { return &module.acc.manualLFOMin; };
  manualLFOMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOMin);
  manualLFOMin.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOMin);
  manualLFOMin.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOMin);
  SetManualParamEnabledWhen(manualLFOMin, FFGlobalUniTarget::LFOMin);

  auto& manualLFOMax = result->params[(int)FFGlobalUniParam::ManualLFOMax];
  manualLFOMax.mode = FBParamMode::Accurate;
  manualLFOMax.defaultText = "50";
  manualLFOMax.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOMax, s); };
  manualLFOMax.slotFormatterOverrides = true;
  manualLFOMax.slotCount = FFGlobalUniMaxCount;
  manualLFOMax.unit = "%";
  manualLFOMax.id = "{D6A4D1B5-8961-4C5B-96CA-792F73227A0C}";
  manualLFOMax.description = "Unison Manual Param LFO Max";
  manualLFOMax.type = FBParamType::Identity;
  auto selectManualLFOMax = [](auto& module) { return &module.acc.manualLFOMax; };
  manualLFOMax.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOMax);
  manualLFOMax.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOMax);
  manualLFOMax.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOMax);
  SetManualParamEnabledWhen(manualLFOMax, FFGlobalUniTarget::LFOMax);

  auto& manualLFOSkewAX = result->params[(int)FFGlobalUniParam::ManualLFOSkewAX];
  manualLFOSkewAX.mode = FBParamMode::Accurate;
  manualLFOSkewAX.defaultText = "50";
  manualLFOSkewAX.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOSkewAX, s); };
  manualLFOSkewAX.slotFormatterOverrides = true;
  manualLFOSkewAX.slotCount = FFGlobalUniMaxCount;
  manualLFOSkewAX.unit = "%";
  manualLFOSkewAX.id = "{5FABD18F-6F53-4532-AC77-7FC5882B2CD1}";
  manualLFOSkewAX.description = "Unison Manual Param LFO A Skew X";
  manualLFOSkewAX.type = FBParamType::Identity;
  auto selectManualLFOSkewAX = [](auto& module) { return &module.acc.manualLFOSkewAX; };
  manualLFOSkewAX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOSkewAX);
  manualLFOSkewAX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOSkewAX);
  manualLFOSkewAX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOSkewAX);
  SetManualParamEnabledWhen(manualLFOSkewAX, FFGlobalUniTarget::LFOSkewAX);

  auto& manualLFOSkewAY = result->params[(int)FFGlobalUniParam::ManualLFOSkewAY];
  manualLFOSkewAY.mode = FBParamMode::Accurate;
  manualLFOSkewAY.defaultText = "50";
  manualLFOSkewAY.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::LFOSkewAY, s); };
  manualLFOSkewAY.slotFormatterOverrides = true;
  manualLFOSkewAY.slotCount = FFGlobalUniMaxCount;
  manualLFOSkewAY.unit = "%";
  manualLFOSkewAY.id = "{7C900D23-EC5F-483B-82A3-EB27E2DD9E66}";
  manualLFOSkewAY.description = "Unison Manual Param LFO A Skew Y";
  manualLFOSkewAY.type = FBParamType::Identity;
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
  manualLFOPhaseOffset.description = "Unison Manual Param LFO Phase Offset";
  manualLFOPhaseOffset.type = FBParamType::Identity;
  auto selectManualLFOPhaseOffset = [](auto& module) { return &module.acc.manualLFOPhaseOffset; };
  manualLFOPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualLFOPhaseOffset);
  manualLFOPhaseOffset.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualLFOPhaseOffset);
  manualLFOPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualLFOPhaseOffset);
  SetManualParamEnabledWhen(manualLFOPhaseOffset, FFGlobalUniTarget::LFOPhaseOffset);

  auto& manualVFXParamA = result->params[(int)FFGlobalUniParam::ManualVFXParamA];
  manualVFXParamA.mode = FBParamMode::Accurate;
  manualVFXParamA.defaultText = "50";
  manualVFXParamA.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamA, s); };
  manualVFXParamA.slotFormatterOverrides = true;
  manualVFXParamA.slotCount = FFGlobalUniMaxCount;
  manualVFXParamA.unit = "%";
  manualVFXParamA.id = "{E383FFF3-6549-44A4-878B-9BE81EB4A69F}";
  manualVFXParamA.description = "Unison Manual Param VFX Control Param A";
  manualVFXParamA.type = FBParamType::Identity;
  auto selectManualVFXParamA = [](auto& module) { return &module.acc.manualVFXParamA; };
  manualVFXParamA.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamA);
  manualVFXParamA.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamA);
  manualVFXParamA.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamA);
  SetManualParamEnabledWhen(manualVFXParamA, FFGlobalUniTarget::VFXParamA);

  auto& manualVFXParamB = result->params[(int)FFGlobalUniParam::ManualVFXParamB];
  manualVFXParamB.mode = FBParamMode::Accurate;
  manualVFXParamB.defaultText = "50";
  manualVFXParamB.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamB, s); };
  manualVFXParamB.slotFormatterOverrides = true;
  manualVFXParamB.slotCount = FFGlobalUniMaxCount;
  manualVFXParamB.unit = "%";
  manualVFXParamB.id = "{82D2D353-D454-496B-BB0E-F7B23D20D698}";
  manualVFXParamB.description = "Unison Manual Param VFX Control Param B";
  manualVFXParamB.type = FBParamType::Identity;
  auto selectManualVFXParamB = [](auto& module) { return &module.acc.manualVFXParamB; };
  manualVFXParamB.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamB);
  manualVFXParamB.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamB);
  manualVFXParamB.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamB);
  SetManualParamEnabledWhen(manualVFXParamB, FFGlobalUniTarget::VFXParamB);

  auto& manualVFXParamC = result->params[(int)FFGlobalUniParam::ManualVFXParamC];
  manualVFXParamC.mode = FBParamMode::Accurate;
  manualVFXParamC.defaultText = "50";
  manualVFXParamC.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamC, s); };
  manualVFXParamC.slotFormatterOverrides = true;
  manualVFXParamC.slotCount = FFGlobalUniMaxCount;
  manualVFXParamC.unit = "%";
  manualVFXParamC.id = "{55539EFD-3561-4F1F-BDDC-FF0CEC2AA14B}";
  manualVFXParamC.description = "Unison Manual Param VFX Control Param C";
  manualVFXParamC.type = FBParamType::Identity;
  auto selectManualVFXParamC = [](auto& module) { return &module.acc.manualVFXParamC; };
  manualVFXParamC.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamC);
  manualVFXParamC.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamC);
  manualVFXParamC.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamC);
  SetManualParamEnabledWhen(manualVFXParamC, FFGlobalUniTarget::VFXParamC);

  auto& manualVFXParamD = result->params[(int)FFGlobalUniParam::ManualVFXParamD];
  manualVFXParamD.mode = FBParamMode::Accurate;
  manualVFXParamD.defaultText = "50";
  manualVFXParamD.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::VFXParamD, s); };
  manualVFXParamD.slotFormatterOverrides = true;
  manualVFXParamD.slotCount = FFGlobalUniMaxCount;
  manualVFXParamD.unit = "%";
  manualVFXParamD.id = "{9346FF6B-068F-4F36-A4CD-68A652CF1A2C}";
  manualVFXParamD.description = "Unison Manual Param VFX Control Param D";
  manualVFXParamD.type = FBParamType::Identity;
  auto selectManualVFXParamD = [](auto& module) { return &module.acc.manualVFXParamD; };
  manualVFXParamD.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualVFXParamD);
  manualVFXParamD.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualVFXParamD);
  manualVFXParamD.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualVFXParamD);
  SetManualParamEnabledWhen(manualVFXParamD, FFGlobalUniTarget::VFXParamD);

  auto& manualEnvSlope = result->params[(int)FFGlobalUniParam::ManualEnvSlope];
  manualEnvSlope.mode = FBParamMode::Accurate;
  manualEnvSlope.defaultText = "50";
  manualEnvSlope.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EnvSlope, s); };
  manualEnvSlope.slotFormatterOverrides = true;
  manualEnvSlope.slotCount = FFGlobalUniMaxCount;
  manualEnvSlope.unit = "%";
  manualEnvSlope.id = "{19A72E15-FC14-4F36-8CB9-C176FB09B78C}";
  manualEnvSlope.description = "Unison Manual Param Envelope Slope";
  manualEnvSlope.type = FBParamType::Identity;
  auto selectManualEnvSlope = [](auto& module) { return &module.acc.manualEnvSlope; };
  manualEnvSlope.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEnvSlope);
  manualEnvSlope.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEnvSlope);
  manualEnvSlope.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEnvSlope);
  SetManualParamEnabledWhen(manualEnvSlope, FFGlobalUniTarget::EnvSlope);

  auto& manualEnvStretch = result->params[(int)FFGlobalUniParam::ManualEnvStretch];
  manualEnvStretch.mode = FBParamMode::VoiceStart;
  manualEnvStretch.defaultText = "50";
  manualEnvStretch.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EnvStretch, s); };
  manualEnvStretch.slotFormatterOverrides = true;
  manualEnvStretch.slotCount = FFGlobalUniMaxCount;
  manualEnvStretch.unit = "%";
  manualEnvStretch.id = "{6155B06D-D928-4EBF-9A52-99EAA3341D26}";
  manualEnvStretch.description = "Unison Manual Param Envelope Stretch";
  manualEnvStretch.type = FBParamType::Identity;
  auto selectManualEnvStretch = [](auto& module) { return &module.acc.manualEnvStretch; };
  manualEnvStretch.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEnvStretch);
  manualEnvStretch.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEnvStretch);
  manualEnvStretch.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEnvStretch);
  SetManualParamEnabledWhen(manualEnvStretch, FFGlobalUniTarget::EnvStretch);

  auto& manualEchoExtend = result->params[(int)FFGlobalUniParam::ManualEchoExtend];
  manualEchoExtend.mode = FBParamMode::VoiceStart;
  manualEchoExtend.defaultText = "50";
  manualEchoExtend.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoExtend, s); };
  manualEchoExtend.slotFormatterOverrides = true;
  manualEchoExtend.slotCount = FFGlobalUniMaxCount;
  manualEchoExtend.unit = "%";
  manualEchoExtend.id = "{B60645F1-F349-4911-AA11-218E188C0D1F}";
  manualEchoExtend.description = "Unison Manual Param VEcho Extend";
  manualEchoExtend.type = FBParamType::Identity;
  auto selectManualEchoExtend = [](auto& module) { return &module.acc.manualEchoExtend; };
  manualEchoExtend.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoExtend);
  manualEchoExtend.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoExtend);
  manualEchoExtend.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoExtend);
  SetManualParamEnabledWhen(manualEchoExtend, FFGlobalUniTarget::EchoExtend);

  auto& manualEchoFade = result->params[(int)FFGlobalUniParam::ManualEchoFade];
  manualEchoFade.mode = FBParamMode::VoiceStart;
  manualEchoFade.defaultText = "50";
  manualEchoFade.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFade, s); };
  manualEchoFade.slotFormatterOverrides = true;
  manualEchoFade.slotCount = FFGlobalUniMaxCount;
  manualEchoFade.unit = "%";
  manualEchoFade.id = "{A9F81A4A-1FB3-418B-86F5-8F49C77934B7}";
  manualEchoFade.description = "Unison Manual Param VEcho Fade";
  manualEchoFade.type = FBParamType::Identity;
  auto selectManualEchoFade = [](auto& module) { return &module.acc.manualEchoFade; };
  manualEchoFade.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFade);
  manualEchoFade.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFade);
  manualEchoFade.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFade);
  SetManualParamEnabledWhen(manualEchoFade, FFGlobalUniTarget::EchoFade);

  auto& manualEchoTapsMix = result->params[(int)FFGlobalUniParam::ManualEchoTapsMix];
  manualEchoTapsMix.mode = FBParamMode::Accurate;
  manualEchoTapsMix.defaultText = "50";
  manualEchoTapsMix.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapsMix, s); };
  manualEchoTapsMix.slotFormatterOverrides = true;
  manualEchoTapsMix.slotCount = FFGlobalUniMaxCount;
  manualEchoTapsMix.unit = "%";
  manualEchoTapsMix.id = "{A480C75C-3429-44A1-BAAF-44341E793F80}";
  manualEchoTapsMix.description = "Unison Manual Param VEcho Taps Dry/Wet Mix";
  manualEchoTapsMix.type = FBParamType::Identity;
  auto selectManualEchoTapsMix = [](auto& module) { return &module.acc.manualEchoTapsMix; };
  manualEchoTapsMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapsMix);
  manualEchoTapsMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapsMix);
  manualEchoTapsMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapsMix);
  SetManualParamEnabledWhen(manualEchoTapsMix, FFGlobalUniTarget::EchoTapsMix);

  auto& manualEchoTapDelay = result->params[(int)FFGlobalUniParam::ManualEchoTapDelay];
  manualEchoTapDelay.mode = FBParamMode::Accurate;
  manualEchoTapDelay.defaultText = "50";
  manualEchoTapDelay.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapDelay, s); };
  manualEchoTapDelay.slotFormatterOverrides = true;
  manualEchoTapDelay.slotCount = FFGlobalUniMaxCount;
  manualEchoTapDelay.unit = "%";
  manualEchoTapDelay.id = "{158E67F4-DD9C-4A2A-93B8-6914B8F9DCED}";
  manualEchoTapDelay.description = "Unison Manual Param VEcho Tap Delay Time";
  manualEchoTapDelay.type = FBParamType::Identity;
  auto selectManualEchoTapDelay = [](auto& module) { return &module.acc.manualEchoTapDelay; };
  manualEchoTapDelay.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapDelay);
  manualEchoTapDelay.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapDelay);
  manualEchoTapDelay.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapDelay);
  SetManualParamEnabledWhen(manualEchoTapDelay, FFGlobalUniTarget::EchoTapDelay);

  auto& manualEchoTapBal = result->params[(int)FFGlobalUniParam::ManualEchoTapBal];
  manualEchoTapBal.mode = FBParamMode::Accurate;
  manualEchoTapBal.defaultText = "50";
  manualEchoTapBal.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapBal, s); };
  manualEchoTapBal.slotFormatterOverrides = true;
  manualEchoTapBal.slotCount = FFGlobalUniMaxCount;
  manualEchoTapBal.unit = "%";
  manualEchoTapBal.id = "{1E8EDB12-2E07-47DE-BB0C-39AC596896E0}";
  manualEchoTapBal.description = "Unison Manual Param VEcho Tap Balance";
  manualEchoTapBal.type = FBParamType::Identity;
  auto selectManualEchoTapBal = [](auto& module) { return &module.acc.manualEchoTapBal; };
  manualEchoTapBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapBal);
  manualEchoTapBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapBal);
  manualEchoTapBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapBal);
  SetManualParamEnabledWhen(manualEchoTapBal, FFGlobalUniTarget::EchoTapBal);

  auto& manualEchoTapLPF = result->params[(int)FFGlobalUniParam::ManualEchoTapLPF];
  manualEchoTapLPF.mode = FBParamMode::Accurate;
  manualEchoTapLPF.defaultText = "50";
  manualEchoTapLPF.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapLPF, s); };
  manualEchoTapLPF.slotFormatterOverrides = true;
  manualEchoTapLPF.slotCount = FFGlobalUniMaxCount;
  manualEchoTapLPF.unit = "%";
  manualEchoTapLPF.id = "{47E81F03-58DC-4A04-9534-5E1A04E1C150}";
  manualEchoTapLPF.description = "Unison Manual Param VEcho Tap LP Frequency";
  manualEchoTapLPF.type = FBParamType::Identity;
  auto selectManualEchoTapLPF = [](auto& module) { return &module.acc.manualEchoTapLPF; };
  manualEchoTapLPF.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapLPF);
  manualEchoTapLPF.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapLPF);
  manualEchoTapLPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapLPF);
  SetManualParamEnabledWhen(manualEchoTapLPF, FFGlobalUniTarget::EchoTapLPF);

  auto& manualEchoTapHPF = result->params[(int)FFGlobalUniParam::ManualEchoTapHPF];
  manualEchoTapHPF.mode = FBParamMode::Accurate;
  manualEchoTapHPF.defaultText = "50";
  manualEchoTapHPF.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoTapHPF, s); };
  manualEchoTapHPF.slotFormatterOverrides = true;
  manualEchoTapHPF.slotCount = FFGlobalUniMaxCount;
  manualEchoTapHPF.unit = "%";
  manualEchoTapHPF.id = "{61C0E876-6480-421F-A5BB-EA9A616751FB}";
  manualEchoTapHPF.description = "Unison Manual Param VEcho Tap HP Frequency";
  manualEchoTapHPF.type = FBParamType::Identity;
  auto selectManualEchoTapHPF = [](auto& module) { return &module.acc.manualEchoTapHPF; };
  manualEchoTapHPF.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoTapHPF);
  manualEchoTapHPF.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoTapHPF);
  manualEchoTapHPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoTapHPF);
  SetManualParamEnabledWhen(manualEchoTapHPF, FFGlobalUniTarget::EchoTapHPF);

  auto& manualEchoFdbkDelay = result->params[(int)FFGlobalUniParam::ManualEchoFdbkDelay];
  manualEchoFdbkDelay.mode = FBParamMode::Accurate;
  manualEchoFdbkDelay.defaultText = "50";
  manualEchoFdbkDelay.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkDelay, s); };
  manualEchoFdbkDelay.slotFormatterOverrides = true;
  manualEchoFdbkDelay.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkDelay.unit = "%";
  manualEchoFdbkDelay.id = "{6D51D872-4BFD-4F06-B202-BD971DB527FA}";
  manualEchoFdbkDelay.description = "Unison Manual Param VEcho Feedback Delay Time";
  manualEchoFdbkDelay.type = FBParamType::Identity;
  auto selectManualEchoFdbkDelay = [](auto& module) { return &module.acc.manualEchoFdbkDelay; };
  manualEchoFdbkDelay.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkDelay);
  manualEchoFdbkDelay.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkDelay);
  manualEchoFdbkDelay.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkDelay);
  SetManualParamEnabledWhen(manualEchoFdbkDelay, FFGlobalUniTarget::EchoFdbkDelay);

  auto& manualEchoFdbkMix = result->params[(int)FFGlobalUniParam::ManualEchoFdbkMix];
  manualEchoFdbkMix.mode = FBParamMode::Accurate;
  manualEchoFdbkMix.defaultText = "50";
  manualEchoFdbkMix.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkMix, s); };
  manualEchoFdbkMix.slotFormatterOverrides = true;
  manualEchoFdbkMix.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkMix.unit = "%";
  manualEchoFdbkMix.id = "{41E192B1-909D-41B6-93B5-9343460670A2}";
  manualEchoFdbkMix.description = "Unison Manual Param VEcho Feedback Dry/Wet Mix";
  manualEchoFdbkMix.type = FBParamType::Identity;
  auto selectManualEchoFdbkMix = [](auto& module) { return &module.acc.manualEchoFdbkMix; };
  manualEchoFdbkMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkMix);
  manualEchoFdbkMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkMix);
  manualEchoFdbkMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkMix);
  SetManualParamEnabledWhen(manualEchoFdbkMix, FFGlobalUniTarget::EchoFdbkMix);

  auto& manualEchoFdbkAmt = result->params[(int)FFGlobalUniParam::ManualEchoFdbkAmt];
  manualEchoFdbkAmt.mode = FBParamMode::Accurate;
  manualEchoFdbkAmt.defaultText = "50";
  manualEchoFdbkAmt.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkAmt, s); };
  manualEchoFdbkAmt.slotFormatterOverrides = true;
  manualEchoFdbkAmt.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkAmt.unit = "%";
  manualEchoFdbkAmt.id = "{BF96F4F2-74C7-4BA8-8FE5-72320499849E}";
  manualEchoFdbkAmt.description = "Unison Manual Param VEcho Feedback Amount";
  manualEchoFdbkAmt.type = FBParamType::Identity;
  auto selectManualEchoFdbkAmt = [](auto& module) { return &module.acc.manualEchoFdbkAmt; };
  manualEchoFdbkAmt.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkAmt);
  manualEchoFdbkAmt.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkAmt);
  manualEchoFdbkAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkAmt);
  SetManualParamEnabledWhen(manualEchoFdbkAmt, FFGlobalUniTarget::EchoFdbkAmt);

  auto& manualEchoFdbkLPF = result->params[(int)FFGlobalUniParam::ManualEchoFdbkLPF];
  manualEchoFdbkLPF.mode = FBParamMode::Accurate;
  manualEchoFdbkLPF.defaultText = "50";
  manualEchoFdbkLPF.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkLPF, s); };
  manualEchoFdbkLPF.slotFormatterOverrides = true;
  manualEchoFdbkLPF.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkLPF.unit = "%";
  manualEchoFdbkLPF.id = "{7F92602A-61F7-42E8-AAE7-9C931B9200E2}";
  manualEchoFdbkLPF.description = "Unison Manual Param VEcho Feedback LP Frequency";
  manualEchoFdbkLPF.type = FBParamType::Identity;
  auto selectManualEchoFdbkLPF = [](auto& module) { return &module.acc.manualEchoFdbkLPF; };
  manualEchoFdbkLPF.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkLPF);
  manualEchoFdbkLPF.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkLPF);
  manualEchoFdbkLPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkLPF);
  SetManualParamEnabledWhen(manualEchoFdbkLPF, FFGlobalUniTarget::EchoFdbkLPF);

  auto& manualEchoFdbkHPF = result->params[(int)FFGlobalUniParam::ManualEchoFdbkHPF];
  manualEchoFdbkHPF.mode = FBParamMode::Accurate;
  manualEchoFdbkHPF.defaultText = "50";
  manualEchoFdbkHPF.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoFdbkHPF, s); };
  manualEchoFdbkHPF.slotFormatterOverrides = true;
  manualEchoFdbkHPF.slotCount = FFGlobalUniMaxCount;
  manualEchoFdbkHPF.unit = "%";
  manualEchoFdbkHPF.id = "{089575E1-8167-46F7-80C7-86BA739693A8}";
  manualEchoFdbkHPF.description = "Unison Manual Param VEcho Feedback HP Frequency";
  manualEchoFdbkHPF.type = FBParamType::Identity;
  auto selectManualEchoFdbkHPF = [](auto& module) { return &module.acc.manualEchoFdbkHPF; };
  manualEchoFdbkHPF.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoFdbkHPF);
  manualEchoFdbkHPF.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoFdbkHPF);
  manualEchoFdbkHPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoFdbkHPF);
  SetManualParamEnabledWhen(manualEchoFdbkHPF, FFGlobalUniTarget::EchoFdbkHPF);

  auto& manualEchoReverbMix = result->params[(int)FFGlobalUniParam::ManualEchoReverbMix];
  manualEchoReverbMix.mode = FBParamMode::Accurate;
  manualEchoReverbMix.defaultText = "50";
  manualEchoReverbMix.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbMix, s); };
  manualEchoReverbMix.slotFormatterOverrides = true;
  manualEchoReverbMix.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbMix.unit = "%";
  manualEchoReverbMix.id = "{475A0982-1DEE-4636-B532-0D0FCD816DEC}";
  manualEchoReverbMix.description = "Unison Manual Param VEcho Reverb Dry/Wet Mix";
  manualEchoReverbMix.type = FBParamType::Identity;
  auto selectManualEchoReverbMix = [](auto& module) { return &module.acc.manualEchoReverbMix; };
  manualEchoReverbMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbMix);
  manualEchoReverbMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbMix);
  manualEchoReverbMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbMix);
  SetManualParamEnabledWhen(manualEchoReverbMix, FFGlobalUniTarget::EchoReverbMix);

  auto& manualEchoReverbSize = result->params[(int)FFGlobalUniParam::ManualEchoReverbSize];
  manualEchoReverbSize.mode = FBParamMode::Accurate;
  manualEchoReverbSize.defaultText = "50";
  manualEchoReverbSize.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbSize, s); };
  manualEchoReverbSize.slotFormatterOverrides = true;
  manualEchoReverbSize.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbSize.unit = "%";
  manualEchoReverbSize.id = "{8E997A4E-BCF6-4599-9329-377E81F77B5F}";
  manualEchoReverbSize.description = "Unison Manual Param VEcho Reverb Room Size";
  manualEchoReverbSize.type = FBParamType::Identity;
  auto selectManualEchoReverbSize = [](auto& module) { return &module.acc.manualEchoReverbSize; };
  manualEchoReverbSize.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbSize);
  manualEchoReverbSize.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbSize);
  manualEchoReverbSize.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbSize);
  SetManualParamEnabledWhen(manualEchoReverbSize, FFGlobalUniTarget::EchoReverbSize);

  auto& manualEchoReverbDamp = result->params[(int)FFGlobalUniParam::ManualEchoReverbDamp];
  manualEchoReverbDamp.mode = FBParamMode::Accurate;
  manualEchoReverbDamp.defaultText = "50";
  manualEchoReverbDamp.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbDamp, s); };
  manualEchoReverbDamp.slotFormatterOverrides = true;
  manualEchoReverbDamp.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbDamp.unit = "%";
  manualEchoReverbDamp.id = "{6E27DACE-DEA9-4168-8752-6BFB0B09002E}";
  manualEchoReverbDamp.description = "Unison Manual Param VEcho Reverb Damping Factor";
  manualEchoReverbDamp.type = FBParamType::Identity;
  auto selectManualEchoReverbDamp = [](auto& module) { return &module.acc.manualEchoReverbDamp; };
  manualEchoReverbDamp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbDamp);
  manualEchoReverbDamp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbDamp);
  manualEchoReverbDamp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbDamp);
  SetManualParamEnabledWhen(manualEchoReverbDamp, FFGlobalUniTarget::EchoReverbDamp);

  auto& manualEchoReverbLPF = result->params[(int)FFGlobalUniParam::ManualEchoReverbLPF];
  manualEchoReverbLPF.mode = FBParamMode::Accurate;
  manualEchoReverbLPF.defaultText = "50";
  manualEchoReverbLPF.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbLPF, s); };
  manualEchoReverbLPF.slotFormatterOverrides = true;
  manualEchoReverbLPF.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbLPF.unit = "%";
  manualEchoReverbLPF.id = "{64120E13-8778-42CF-8A7D-EB3EC175DA15}";
  manualEchoReverbLPF.description = "Unison Manual Param VEcho Reverb LP Frequency";
  manualEchoReverbLPF.type = FBParamType::Identity;
  auto selectManualEchoReverbLPF = [](auto& module) { return &module.acc.manualEchoReverbLPF; };
  manualEchoReverbLPF.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbLPF);
  manualEchoReverbLPF.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbLPF);
  manualEchoReverbLPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbLPF);
  SetManualParamEnabledWhen(manualEchoReverbLPF, FFGlobalUniTarget::EchoReverbLPF);

  auto& manualEchoReverbHPF = result->params[(int)FFGlobalUniParam::ManualEchoReverbHPF];
  manualEchoReverbHPF.mode = FBParamMode::Accurate;
  manualEchoReverbHPF.defaultText = "50";
  manualEchoReverbHPF.slotFormatter = [](auto const&, auto, int s) { return ManualSlotFormatter(FFGlobalUniTarget::EchoReverbHPF, s); };
  manualEchoReverbHPF.slotFormatterOverrides = true;
  manualEchoReverbHPF.slotCount = FFGlobalUniMaxCount;
  manualEchoReverbHPF.unit = "%";
  manualEchoReverbHPF.id = "{87EC2DB4-38C8-4EB3-9A1E-E2BABD327BF7}";
  manualEchoReverbHPF.description = "Unison Manual Param VEcho Reverb HP Frequency";
  manualEchoReverbHPF.type = FBParamType::Identity;
  auto selectManualEchoReverbHPF = [](auto& module) { return &module.acc.manualEchoReverbHPF; };
  manualEchoReverbHPF.scalarAddr = FFSelectScalarParamAddr(selectModule, selectManualEchoReverbHPF);
  manualEchoReverbHPF.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectManualEchoReverbHPF);
  manualEchoReverbHPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectManualEchoReverbHPF);
  SetManualParamEnabledWhen(manualEchoReverbHPF, FFGlobalUniTarget::EchoReverbHPF);

  return result;
}