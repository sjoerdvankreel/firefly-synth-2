#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOGraph.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeLFOBarsItems(bool withZero)
{
  return FBMakeBarsItems(withZero, { 1, 128 }, { 4, 1 });
}

std::unique_ptr<FBStaticModule>
FFMakeLFOTopo(bool global)
{
  std::string prefix = global ? "G" : "V";
  auto result = std::make_unique<FBStaticModule>();
  result->voice = !global;
  result->name = global ? "Global LFO" : "Voice LFO";
  result->tabName = global ? "GLFO" : "VLFO";
  result->graphName = global ? "GLFO" : "VLFO";
  result->slotCount = FFLFOCount;
  result->graphCount = FFLFOBlockCount + 1;
  result->graphRenderer = global ? FFLFORenderGraph<true> : FFLFORenderGraph<false>;
  result->id = prefix + "{6E9EC930-5391-41BB-9EDA-C9B79F3BE745}";
  result->params.resize((int)FFLFOParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vLFO; });
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gLFO; });
  auto selectVoiceModule = [](auto& state) { return &state.voice.vLFO; };
  auto selectGlobalModule = [](auto& state) { return &state.global.gLFO; };

  auto& type = result->params[(int)FFLFOParam::Type];
  type.acc = false;
  type.defaultText = "Off";
  type.name = "Type";
  type.slotCount = 1;
  type.id = prefix + "{D75D506B-3976-44C3-A609-3982D26B4F2D}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{EBD31B78-C404-4B86-A8BA-48788922F34B}" },
    { "{20AA6ED6-A5F3-491D-9485-FBA7F2186632}" },
    { "{6AFA440C-BBB6-4C19-A15B-1CEE038DE435}" } };
  type.List().slotFormatter = [](int mi, int is)
  {
      if (is == 0) return "Off";
      if (is == 1) return "Free";
      if (is == 2)
        if (mi == (int)FFModuleType::GLFO)
          return "Snap";
        else
        {
          FB_ASSERT(mi == (int)FFModuleType::VLFO);
          return "One Shot";
        }
      FB_ASSERT(false);
      return "";
  };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectType);
  type.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectType);
  type.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectType);

  auto& sync = result->params[(int)FFLFOParam::Sync];
  sync.acc = false;
  sync.display = "Sync";
  sync.name = "Tempo Sync";
  sync.slotCount = 1;
  sync.defaultText = "Off";
  sync.id = prefix + "{879655C7-41EC-4089-84E0-A5E244E47F56}";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSync);
  sync.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSync);
  sync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSync);
  sync.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSync);
  sync.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSync);
  sync.dependencies.enabled.audio.When({ (int)FFLFOParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& seed = result->params[(int)FFLFOParam::Seed];
  seed.acc = false;
  seed.defaultText = "0";
  seed.name = "Seed";
  seed.slotCount = 1;
  seed.id = prefix + "{3283748D-70B7-41FC-986B-1B76D4E35F48}";
  seed.type = FBParamType::Discrete;
  seed.Discrete().valueCount = FFLFOMaxSeed + 1;
  auto selectSeed = [](auto& module) { return &module.block.seed; };
  seed.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSeed);
  seed.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSeed);
  seed.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSeed);
  seed.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSeed);
  seed.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSeed);
  seed.dependencies.enabled.audio.When({ (int)FFLFOParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& smoothTime = result->params[(int)FFLFOParam::SmoothTime];
  smoothTime.acc = false;
  smoothTime.defaultText = "0";
  smoothTime.display = "Smth";
  smoothTime.name = "Smooth Time";
  smoothTime.slotCount = 1;
  smoothTime.unit = "Sec";
  smoothTime.id = prefix + "{6064C303-9588-433E-84FE-79B61FDD2ABE}";
  smoothTime.type = FBParamType::Linear;
  smoothTime.Linear().min = 0.0f;
  smoothTime.Linear().max = 10.0f;
  smoothTime.Linear().editSkewFactor = 0.5f;
  auto selectSmoothTime = [](auto& module) { return &module.block.smoothTime; };
  smoothTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSmoothTime);
  smoothTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSmoothTime);
  smoothTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSmoothTime);
  smoothTime.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSmoothTime);
  smoothTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSmoothTime);
  smoothTime.dependencies.visible.audio.When({ (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  smoothTime.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& smoothBars = result->params[(int)FFLFOParam::SmoothBars];
  smoothBars.acc = false;
  smoothBars.defaultText = "Off";
  smoothBars.display = "Smth";
  smoothBars.name = "Smooth Bars";
  smoothBars.slotCount = 1;
  smoothBars.unit = "Bars";
  smoothBars.id = prefix + "{B1FEDD9E-47D0-4D3D-981C-E7C8D8A65BC7}";
  smoothBars.type = FBParamType::Bars;
  smoothBars.Bars().items = MakeLFOBarsItems(true);
  auto selectSmoothBars = [](auto& module) { return &module.block.smoothBars; };
  smoothBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSmoothBars);
  smoothBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSmoothBars);
  smoothBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSmoothBars);
  smoothBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSmoothBars);
  smoothBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSmoothBars);
  smoothBars.dependencies.visible.audio.When({ (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  smoothBars.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& opType = result->params[(int)FFLFOParam::OpType];
  opType.acc = false;
  opType.name = "Op";
  opType.display = "Op";
  opType.slotCount = FFLFOBlockCount;
  opType.slotFormatter = FFFormatBlockSlot;
  opType.id = prefix + "{B60CF69F-B21F-4BB6-891A-9E1493D0E40E}";
  opType.defaultTextSelector = [](int /*mi*/, int, int ps) { return ps == 0 ? "Add" : "Off"; };
  opType.type = FBParamType::List;
  opType.List().items = {
    { "{A1E456A1-05D9-4915-8C90-0076FFD9DADF}", "Off" },
    { "{68818E5D-62D3-433A-A81A-7FAA7EA11018}", "Add" },
    { "{AD641260-F205-497E-B483-330CFA025378}", "Mul" },
    { "{5D97E841-675B-423F-B30C-06AD60AC0A54}", "Stk" } };
  auto selectOpType = [](auto& module) { return &module.block.opType; };
  opType.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOpType);
  opType.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOpType);
  opType.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOpType);
  opType.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOpType);
  opType.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOpType);
  opType.dependencies.enabled.audio.When({ (int)FFLFOParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& min = result->params[(int)FFLFOParam::Min];
  min.acc = true;
  min.defaultText = "0";
  min.name = "Min";
  min.display = "Min";
  min.slotCount = FFLFOBlockCount;
  min.slotFormatter = FFFormatBlockSlot;
  min.unit = "%";
  min.id = prefix + "{6E12AC6E-B1C3-4DA4-A1C0-EEB7C2187208}";
  min.type = FBParamType::Identity;
  auto selectMin = [](auto& module) { return &module.acc.min; };
  min.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectMin);
  min.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectMin);
  min.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectMin);
  min.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectMin);
  min.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectMin);
  min.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::OpType }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& max = result->params[(int)FFLFOParam::Max];
  max.acc = true;
  max.defaultText = "100";
  max.name = "Max";
  max.display = "Max";
  max.slotCount = FFLFOBlockCount;
  max.slotFormatter = FFFormatBlockSlot;
  max.unit = "%";
  max.id = prefix + "{C6DC5AF0-39EF-4C85-8FD6-6A0FDFC88732}";
  max.type = FBParamType::Identity;
  auto selectMax = [](auto& module) { return &module.acc.max; };
  max.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectMax);
  max.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectMax);
  max.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectMax);
  max.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectMax);
  max.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectMax);
  max.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::OpType }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& waveMode = result->params[(int)FFLFOParam::WaveMode];
  waveMode.acc = false;
  waveMode.defaultText = "Sin";
  waveMode.name = "Wave Mode";
  waveMode.display = "Mode";
  waveMode.slotCount = FFLFOBlockCount;
  waveMode.slotFormatter = FFFormatBlockSlot;
  waveMode.id = prefix + "{140C3465-BD6A-495A-BA65-17A82290571E}";
  waveMode.type = FBParamType::List;
  waveMode.List().items = {
    { "{9BD329A7-3F02-4DDC-8A42-C21D6D780C28}", "Sin" },
    { "{1A4E2DDB-2C58-4670-89BC-54AE6CD6EBC2}", "Cos" },
    { "{9CF1B435-0A87-4F41-BA5A-7A596ED6ABC3}", "Sin2" },
    { "{B1589DBB-756C-4EB2-B36C-758F45A42CBD}", "Cos2" },
    { "{1948DE4C-FDE5-471E-A4B1-2FCCA23AA877}", "SnCs" },
    { "{FB7AAA67-5893-466C-9EF8-0D2E1ACC50D1}", "CsSn" },
    { "{F2C4032D-EAE9-490F-A58C-BF7F467260F4}", "Sin3" },
    { "{8E871CFE-2311-45B7-9E96-AB531F0E7E30}", "Cos3" },
    { "{A679AA50-A2EC-4D2F-AB10-A1185745287A}", "S2Cs" },
    { "{90D388CD-38C8-4D7C-BAC5-0492274089DF}", "C2Sn" },
    { "{5237B3A9-A85F-4392-8A0C-340B1A6A3E1B}", "SnC2" },
    { "{7E804C32-169F-43C4-B87C-062FCB60DF26}", "CsS2" },
    { "{CBCB1AF6-4CA2-4381-A5B8-42A7D2F92A36}", "SCS" },
    { "{B6E84130-CC0D-4270-B901-C69E778DAFD8}", "CSC" },
    { "{0402BC62-9213-41D2-AFA3-885B3B0B046D}", "Saw" },
    { "{78CD93BD-B3EF-41BA-A36B-BB24791D31F6}", "Tri" },
    { "{BE0EF052-FA6B-422A-BF2B-6B9BE3430F9D}", "Sqr" },
    { "{BDDA97A8-05A0-4E50-A775-5BA66A9D25C8}", "Rnd" },
    { "{B6B47502-BA24-42A6-80D7-D41FE62F06DF}", "RndF" },
    { "{3D6C2670-3862-4E1D-9010-0D9A36DB5B26}", "Smth" },
    { "{B48C5047-3ABD-4D03-86C9-0B4700E6B035}", "SmthF" } };
  waveMode.List().submenuStart[FFTrigSin] = "Trig1";
  waveMode.List().submenuStart[FFTrigSin2] = "Trig2";
  waveMode.List().submenuStart[FFTrigSin3] = "Trig3";
  waveMode.List().submenuStart[FFLFOWaveModeSaw] = "Other";
  waveMode.List().submenuStart[FFLFOWaveModeRandom] = "Random";
  auto selectWaveMode = [](auto& module) { return &module.block.waveMode; };
  waveMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectWaveMode);
  waveMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectWaveMode);
  waveMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectWaveMode);
  waveMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectWaveMode);
  waveMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectWaveMode);
  waveMode.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::OpType }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& rateHz = result->params[(int)FFLFOParam::RateHz];
  rateHz.acc = true;
  rateHz.defaultText = "1";
  rateHz.display = "Rate";
  rateHz.name = "Rate Hz";
  rateHz.slotCount = FFLFOBlockCount;
  rateHz.slotFormatter = FFFormatBlockSlot;
  rateHz.unit = "Hz";
  rateHz.id = prefix + "{EFAAB971-9F51-4FFD-9873-D33D4591F606}";
  rateHz.type = FBParamType::Linear;
  rateHz.Linear().min = 0.05f;
  rateHz.Linear().max = 20.0f;
  rateHz.Linear().editSkewFactor = 0.5f;
  auto selectRateHz = [](auto& module) { return &module.acc.rateHz; };
  rateHz.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectRateHz);
  rateHz.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectRateHz);
  rateHz.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectRateHz);
  rateHz.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectRateHz);
  rateHz.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectRateHz);
  rateHz.dependencies.visible.audio.When({ (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  rateHz.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::Sync, (int)FFLFOParam::OpType }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0 && vs[2] != 0; });

  auto& rateBars = result->params[(int)FFLFOParam::RateBars];
  rateBars.acc = false;
  rateBars.defaultText = "1/4";
  rateBars.display = "Rate";
  rateBars.name = "Rate Bars";
  rateBars.slotCount = FFLFOBlockCount;
  rateBars.slotFormatter = FFFormatBlockSlot;
  rateBars.unit = "Bars";
  rateBars.id = prefix + "{394760D6-D4CE-453F-9C95-29B788E1E284}";
  rateBars.type = FBParamType::Bars;
  rateBars.Bars().items = MakeLFOBarsItems(false);
  auto selectRateBars = [](auto& module) { return &module.block.rateBars; };
  rateBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectRateBars);
  rateBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectRateBars);
  rateBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectRateBars);
  rateBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectRateBars);
  rateBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectRateBars);
  rateBars.dependencies.visible.audio.When({ (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  rateBars.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::Sync, (int)FFLFOParam::OpType }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& steps = result->params[(int)FFLFOParam::Steps];
  steps.acc = false;
  steps.defaultText = "1";
  steps.name = "Steps";
  steps.display = "Stp";
  steps.slotCount = FFLFOBlockCount;
  steps.slotFormatter = FFFormatBlockSlot;
  steps.id = prefix + "{F356CD96-80FD-4A45-A2BE-76785CC5463F}";
  steps.type = FBParamType::Discrete;
  steps.Discrete().valueCount = 32;
  steps.Discrete().valueOffset = 1;
  auto selectSteps = [](auto& module) { return &module.block.steps; };
  steps.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSteps);
  steps.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSteps);
  steps.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSteps);
  steps.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSteps);
  steps.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSteps);
  steps.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::OpType }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& skewAXMode = result->params[(int)FFLFOParam::SkewAXMode];
  skewAXMode.acc = false;
  skewAXMode.defaultText = "Off";
  skewAXMode.display = "SkX";
  skewAXMode.name = "Skew A X Mode";
  skewAXMode.slotCount = 1;
  skewAXMode.id = prefix + "{AA602AF4-882F-49E0-AA2B-B4D00C1723C2}";
  skewAXMode.type = FBParamType::List;
  skewAXMode.List().items = {
    { "{D057104A-C083-4BA4-9799-63307147B2E2}", "Off" },
    { "{D1FD1E7B-E20F-47DA-9FB8-F03DE80BB109}", "SclU" },
    { "{1556925C-34F8-44F7-A1C9-62E4C4A40265}", "SclB" },
    { "{ECCADB9A-4735-4916-93B0-BB179C78247B}", "ExpU" },
    { "{E2E3B133-B375-4F83-BAE3-EEAD6FF10FF8}", "ExpB" } };
  auto selectSkewAXMode = [](auto& module) { return &module.block.skewAXMode; };
  skewAXMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewAXMode);
  skewAXMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewAXMode);
  skewAXMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewAXMode);
  skewAXMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewAXMode);
  skewAXMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewAXMode);
  skewAXMode.dependencies.enabled.audio.When({ { (int)FFLFOParam::Type, 0 }, { (int)FFLFOParam::OpType, 0 } }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& skewAXAmt = result->params[(int)FFLFOParam::SkewAXAmt];
  skewAXAmt.acc = true;
  skewAXAmt.defaultText = "0";
  skewAXAmt.name = "Skew A X Amt";
  skewAXAmt.slotCount = 1;
  skewAXAmt.unit = "%";
  skewAXAmt.id = prefix + "{49209170-5BA6-4B96-8E60-79A287680EAD}";
  skewAXAmt.type = FBParamType::Identity;
  auto selectSkewAXAmt = [](auto& module) { return &module.acc.skewAXAmt; };
  skewAXAmt.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewAXAmt);
  skewAXAmt.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewAXAmt);
  skewAXAmt.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewAXAmt);
  skewAXAmt.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewAXAmt);
  skewAXAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewAXAmt);
  skewAXAmt.dependencies.enabled.audio.When({ { (int)FFLFOParam::Type, 0 }, { (int)FFLFOParam::OpType, 0 }, { (int)FFLFOParam::SkewAXMode, 0 } }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& skewAYMode = result->params[(int)FFLFOParam::SkewAYMode];
  skewAYMode.acc = false;
  skewAYMode.defaultText = "Off";
  skewAYMode.display = "SkY";
  skewAYMode.name = "Skew Y Mode";
  skewAYMode.slotCount = 1;
  skewAYMode.id = prefix + "{20D3F79F-F727-4164-AE04-27D9D254CE60}";
  skewAYMode.type = FBParamType::List;
  skewAYMode.List().items = {
    { "{ADE99968-98D3-4314-BDE4-09A440FADB45}", "Off" },
    { "{077725EA-4293-48A3-8D1B-6EE452327255}", "ExpU" },
    { "{47B2CD10-27B4-40BE-AD54-053F4DCBCFA0}", "ExpB" } };
  auto selectSkewAYMode = [](auto& module) { return &module.block.skewAYMode; };
  skewAYMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewAYMode);
  skewAYMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewAYMode);
  skewAYMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewAYMode);
  skewAYMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewAYMode);
  skewAYMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewAYMode);
  skewAYMode.dependencies.enabled.audio.When({ { (int)FFLFOParam::Type, 0 }, { (int)FFLFOParam::OpType, 0 } }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& skewAYAmt = result->params[(int)FFLFOParam::SkewAYAmt];
  skewAYAmt.acc = true;
  skewAYAmt.defaultText = "0";
  skewAYAmt.name = "Skew Y Amt";
  skewAYAmt.slotCount = 1;
  skewAYAmt.unit = "%";
  skewAYAmt.id = prefix + "{E8E4BD9E-7E2A-4B26-AA42-87157C5246BF}";
  skewAYAmt.type = FBParamType::Identity;
  auto selectSkewAYAmt = [](auto& module) { return &module.acc.skewAYAmt; };
  skewAYAmt.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewAYAmt);
  skewAYAmt.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewAYAmt);
  skewAYAmt.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewAYAmt);
  skewAYAmt.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewAYAmt);
  skewAYAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewAYAmt);
  skewAYAmt.dependencies.enabled.audio.When({ { (int)FFLFOParam::Type, 0 }, { (int)FFLFOParam::OpType, 0 }, { (int)FFLFOParam::SkewAYMode, 0 } }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& phaseB = result->params[(int)FFLFOParam::PhaseB];
  phaseB.acc = false;
  phaseB.defaultText = "0";
  phaseB.name = "Phase B";
  phaseB.display = "Phs";
  phaseB.slotCount = 1;
  phaseB.unit = "%";
  phaseB.id = prefix + "{4BFEC447-4A16-4AE4-9E73-4FDC889046D1}";
  phaseB.type = FBParamType::Identity;
  auto selectPhaseB = [](auto& module) { return &module.block.phaseB; };
  phaseB.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectPhaseB);
  phaseB.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectPhaseB);
  phaseB.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectPhaseB);
  phaseB.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectPhaseB);
  phaseB.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectPhaseB);
  phaseB.dependencies.enabled.audio.When({ { (int)FFLFOParam::Type, 0 }, { (int)FFLFOParam::OpType, 1 } }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  return result;
}