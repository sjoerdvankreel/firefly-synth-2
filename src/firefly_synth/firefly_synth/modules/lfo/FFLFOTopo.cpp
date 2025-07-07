#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
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
  smoothBars.dependencies.enabled.audio.When({ (int)FFLFOParam::WaveMode, (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& skewXMode = result->params[(int)FFLFOParam::SkewXMode];
  skewXMode.acc = false;
  skewXMode.defaultText = "Off";
  skewXMode.display = "SkX";
  skewXMode.name = "Skew X Mode";
  skewXMode.slotCount = 1;
  skewXMode.id = prefix + "{AA602AF4-882F-49E0-AA2B-B4D00C1723C2}";
  skewXMode.type = FBParamType::List;
  skewXMode.List().items = {
    { "{D057104A-C083-4BA4-9799-63307147B2E2}", "Off" },
    { "{D1FD1E7B-E20F-47DA-9FB8-F03DE80BB109}", "SclU" },
    { "{1556925C-34F8-44F7-A1C9-62E4C4A40265}", "SclB" },
    { "{ECCADB9A-4735-4916-93B0-BB179C78247B}", "ExpU" },
    { "{E2E3B133-B375-4F83-BAE3-EEAD6FF10FF8}", "ExpB" } };
  auto selectSkewXMode = [](auto& module) { return &module.block.skewXMode; };
  skewXMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewXMode);
  skewXMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewXMode);
  skewXMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewXMode);
  skewXMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewXMode);
  skewXMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewXMode);

  auto& skewXAmt = result->params[(int)FFLFOParam::SkewXAmt];
  skewXAmt.acc = true;
  skewXAmt.defaultText = "0";
  skewXAmt.name = "Skew X Amt";
  skewXAmt.slotCount = 1;
  skewXAmt.unit = "%";
  skewXAmt.id = prefix + "{49209170-5BA6-4B96-8E60-79A287680EAD}";
  skewXAmt.type = FBParamType::Identity;
  auto selectSkewXAmt = [](auto& module) { return &module.acc.skewXAmt; };
  skewXAmt.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewXAmt);
  skewXAmt.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewXAmt);
  skewXAmt.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewXAmt);
  skewXAmt.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewXAmt);
  skewXAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewXAmt);

  auto& skewYMode = result->params[(int)FFLFOParam::SkewYMode];
  skewYMode.acc = false;
  skewYMode.defaultText = "Off";
  skewYMode.display = "SkY";
  skewYMode.name = "Skew Y Mode";
  skewYMode.slotCount = 1;
  skewYMode.id = prefix + "{20D3F79F-F727-4164-AE04-27D9D254CE60}";
  skewYMode.type = FBParamType::List;
  skewYMode.List().items = {
    { "{ADE99968-98D3-4314-BDE4-09A440FADB45}", "Off" },
    { "{193A736F-073C-42DC-88EB-140D5D00BF87}", "SclU" },
    { "{B3C72EC2-265B-4D8B-9295-AF3604A5D9CA}", "SclBi" },
    { "{077725EA-4293-48A3-8D1B-6EE452327255}", "ExpU" },
    { "{47B2CD10-27B4-40BE-AD54-053F4DCBCFA0}", "ExpB" } };
  auto selectSkewYMode = [](auto& module) { return &module.block.skewYMode; };
  skewYMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewYMode);
  skewYMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewYMode);
  skewYMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewYMode);
  skewYMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewYMode);
  skewYMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewYMode);

  auto& skewYAmt = result->params[(int)FFLFOParam::SkewYAmt];
  skewYAmt.acc = true;
  skewYAmt.defaultText = "0";
  skewYAmt.name = "Skew Y Amt";
  skewYAmt.slotCount = 1;
  skewYAmt.unit = "%";
  skewYAmt.id = prefix + "{E8E4BD9E-7E2A-4B26-AA42-87157C5246BF}";
  skewYAmt.type = FBParamType::Identity;
  auto selectSkewYAmt = [](auto& module) { return &module.acc.skewYAmt; };
  skewYAmt.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewYAmt);
  skewYAmt.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewYAmt);
  skewYAmt.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewYAmt);
  skewYAmt.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewYAmt);
  skewYAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewYAmt);

  auto& opType = result->params[(int)FFLFOParam::OpType];
  opType.acc = false;
  opType.name = "Op";
  opType.display = "Op";
  opType.slotCount = FFLFOBlockCount;
  opType.id = prefix + "{B60CF69F-B21F-4BB6-891A-9E1493D0E40E}";
  opType.defaultTextSelector = [](int /*mi*/, int, int ps) { return ps == 0 ? "Add" : "Off"; };
  opType.type = FBParamType::List;
  opType.List().items = {
    { "{A1E456A1-05D9-4915-8C90-0076FFD9DADF}", "Off" },
    { "{68818E5D-62D3-433A-A81A-7FAA7EA11018}", "Add" },
    { "{AD641260-F205-497E-B483-330CFA025378}", "Mul" },
    { "{264BC462-B9F4-407E-BFFD-6A50B157C21E}", "Stk" } };
  auto selectOpType = [](auto& module) { return &module.block.opType; };
  opType.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOpType);
  opType.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOpType);
  opType.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOpType);
  opType.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOpType);
  opType.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOpType);

  auto& scale = result->params[(int)FFLFOParam::Scale];
  scale.acc = true;
  scale.defaultText = "0";
  scale.name = "Scale";
  scale.display = "Scl";
  scale.slotCount = FFLFOBlockCount;
  scale.unit = "%";
  scale.id = prefix + "{6E12AC6E-B1C3-4DA4-A1C0-EEB7C2187208}";
  scale.type = FBParamType::Identity;
  auto selectScale = [](auto& module) { return &module.acc.scale; };
  scale.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectScale);
  scale.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectScale);
  scale.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectScale);
  scale.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectScale);
  scale.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectScale);  

  auto& waveMode = result->params[(int)FFLFOParam::WaveMode];
  waveMode.acc = false;
  waveMode.defaultText = "Sin";
  waveMode.name = "Wave Mode";
  waveMode.display = "Mode";
  waveMode.slotCount = FFLFOBlockCount;
  waveMode.id = prefix + "{140C3465-BD6A-495A-BA65-17A82290571E}";
  waveMode.type = FBParamType::List;
  waveMode.List().items = {
    { "{CEFDD4B5-6BE3-44B8-8420-1B9AC59B59FE}", "Sin" },
    { "{54EF10FA-BF52-4C6A-B00F-F47BF2CE6FB5}", "Cos" } };
  auto selectWaveMode = [](auto& module) { return &module.block.waveMode; };
  waveMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectWaveMode);
  waveMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectWaveMode);
  waveMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectWaveMode);
  waveMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectWaveMode);
  waveMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectWaveMode);

  auto& rateTime = result->params[(int)FFLFOParam::RateTime];
  rateTime.acc = true;
  rateTime.defaultText = "1";
  rateTime.display = "Rate";
  rateTime.name = "Rate Time";
  rateTime.slotCount = FFLFOBlockCount;
  rateTime.unit = "Sec";
  rateTime.id = prefix + "{EFAAB971-9F51-4FFD-9873-D33D4591F606}";
  rateTime.type = FBParamType::Linear;
  rateTime.Linear().min = 0.05f;
  rateTime.Linear().max = 10.0f;
  rateTime.Linear().editSkewFactor = 0.5f;
  auto selectRateTime = [](auto& module) { return &module.acc.rateTime; };
  rateTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectRateTime);
  rateTime.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectRateTime);
  rateTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectRateTime);
  rateTime.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectRateTime);
  rateTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectRateTime);
  rateTime.dependencies.visible.audio.When({ (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  rateTime.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& rateBars = result->params[(int)FFLFOParam::RateBars];
  rateBars.acc = false;
  rateBars.defaultText = "1/4";
  rateBars.display = "Rate";
  rateBars.name = "Rate Bars";
  rateBars.slotCount = FFLFOBlockCount;
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
  rateBars.dependencies.enabled.audio.When({ (int)FFLFOParam::WaveMode, (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });  

  auto& phase = result->params[(int)FFLFOParam::Phase];
  phase.acc = false;
  phase.defaultText = "0";
  phase.name = "Phase";
  phase.display = "Phase";
  phase.slotCount = FFLFOBlockCount;
  phase.unit = "%";
  phase.id = prefix + "{88E80B77-86BF-49B0-9822-AACEFC6EAB03}";
  phase.type = FBParamType::Identity;
  auto selectPhase = [](auto& module) { return &module.block.phase; };
  phase.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectPhase);
  phase.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectPhase);
  phase.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectPhase);
  phase.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectPhase);
  phase.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectPhase);

  auto& steps = result->params[(int)FFLFOParam::Steps];
  steps.acc = false;
  steps.defaultText = "1";
  steps.name = "Steps";
  steps.display = "Steps";
  steps.slotCount = FFLFOBlockCount;
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
  
  // todo 'A' 'B'
  // todo all the enabled stuff
  // todo add/mul/stack + drop stack + something bipolar
  return result;
}