#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/shared/FFOscisGraph.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::string
FFOsciFMFormatRatioSlot(FBStaticTopo const&, int slot)
{
  switch (slot)
  {
  case 0: return "2:1";
  case 1: return "3:2";
  default: FB_ASSERT(false); return "";
  }
}

static std::string
FFOsciFMFormatRatioSubMenu(int subMenu)
{
  FB_ASSERT(0 <= subMenu && subMenu < FFOsciFMRatioCount);
  return std::to_string(subMenu + 1);
}

static std::string
FFOsciFMFormatRatioValue(int val)
{
  FB_ASSERT(0 <= val && val < FFOsciFMRatioCount * FFOsciFMRatioCount);
  return std::to_string(val / FFOsciFMRatioCount + 1) + ":" +
    std::to_string(val % FFOsciFMRatioCount + 1);
}

static std::string
FFOsciFMFormatIndexSlot(FBStaticTopo const&, int slot)
{
  FB_ASSERT(0 <= slot && slot < FFOsciFMMatrixSize);
  return std::to_string(slot / FFOsciFMOperatorCount + 1) + "\U00002192" + 
    std::to_string(slot % FFOsciFMOperatorCount + 1);
}

std::unique_ptr<FBStaticModule>
FFMakeOsciTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc";
  result->tabName = "OSC";
  result->graphName = "OSC";
  result->slotCount = FFOsciCount;
  result->graphCount = FFOsciCount + FFStringOsciCount;
  result->graphRenderer = FFOscisRenderGraph;
  result->id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  result->params.resize((int)FFOsciParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.osci; });
  auto selectModule = [](auto& state) { return &state.voice.osci; };

  auto& type = result->params[(int)FFOsciParam::Type];
  type.acc = false;
  type.defaultText = "Off";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{9018865F-7B05-4835-B541-95014C0C63E6}";
  type.defaultTextSelector = [](int ms, int /*ps*/) { return ms == 0 ? "Wave" : "Off"; };
  type.type = FBParamType::List;
  type.List().items = {
    { "{449E467A-2DC0-43B0-8487-57C4492F9FE2}", "Off" },
    { "{3F55D6D7-5BDF-4B7F-B1E0-2E59B96EA5C0}", "Wave" },
    { "{83E9DBC4-5CBF-4C96-93EB-AB16C2E7C769}", "FM" } ,
    { "{F4095D2B-688D-4A89-82FE-359A8902963C}", "String" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);

  auto& gain = result->params[(int)FFOsciParam::Gain];
  gain.acc = true;
  gain.defaultText = "100";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.unit = "%";
  gain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  gain.type = FBParamType::Identity;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectGain);
  gain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGain);
  gain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& coarse = result->params[(int)FFOsciParam::Coarse];
  coarse.acc = true;
  coarse.defaultText = "0";
  coarse.name = "Coarse";
  coarse.slotCount = 1;
  coarse.unit = "Semitones";
  coarse.id = "{E122CA2C-C1B1-47E5-A1BB-DEAC6A4030E0}";
  coarse.type = FBParamType::Linear;
  coarse.Linear().min = -24.0f;
  coarse.Linear().max = 24.0f;
  auto selectCoarse = [](auto& module) { return &module.acc.coarse; };
  coarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCoarse);
  coarse.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCoarse);
  coarse.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCoarse);
  coarse.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& fine = result->params[(int)FFOsciParam::Fine];
  fine.acc = true;
  fine.defaultText = "0";
  fine.name = "Fine";
  fine.slotCount = 1;
  fine.unit = "Cent";
  fine.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  fine.type = FBParamType::Linear;
  fine.Linear().min = -1.0f;
  fine.Linear().max = 1.0f;
  fine.Linear().displayMultiplier = 100.0f;
  auto selectFine = [](auto& module) { return &module.acc.fine; };
  fine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFine);
  fine.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectFine);
  fine.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFine);
  fine.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& uniCount = result->params[(int)FFOsciParam::UniCount];
  uniCount.acc = false;
  uniCount.defaultText = "1";
  uniCount.display = "Uni";
  uniCount.name = "Unison Count";
  uniCount.slotCount = 1;
  uniCount.id = "{60313673-95FE-4B6D-99A6-B628ACDE6D56}";
  uniCount.type = FBParamType::Discrete;
  uniCount.Discrete().valueCount = FFOsciBaseUniMaxCount;
  uniCount.Discrete().valueOffset = 1;
  auto selectUniCount = [](auto& module) { return &module.block.uniCount; };
  uniCount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniCount);
  uniCount.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniCount);
  uniCount.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniCount);
  uniCount.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& uniOffset = result->params[(int)FFOsciParam::UniOffset];
  uniOffset.acc = false;
  uniOffset.defaultText = "50";
  uniOffset.display = "Ofst";
  uniOffset.name = "Unison Offset";
  uniOffset.slotCount = 1;
  uniOffset.unit = "%";
  uniOffset.id = "{6F5754E1-BDF4-4685-98FC-8C613128EE8D}";
  uniOffset.type = FBParamType::Identity;
  auto selectUniOffset = [](auto& module) { return &module.block.uniOffset; };
  uniOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOffset);
  uniOffset.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniOffset);
  uniOffset.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOffset);
  uniOffset.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 1; });

  auto& uniRandom = result->params[(int)FFOsciParam::UniRandom];
  uniRandom.acc = false;
  uniRandom.defaultText = "50";
  uniRandom.display = "Rand";
  uniRandom.name = "Unison Random";
  uniRandom.slotCount = 1;
  uniRandom.unit = "%";
  uniRandom.id = "{6F7F6D55-5740-44AB-8442-267A5730E2DA}";
  uniRandom.type = FBParamType::Identity;
  auto selectUniRandom = [](auto& module) { return &module.block.uniRandom; };
  uniRandom.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniRandom);
  uniRandom.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniRandom);
  uniRandom.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniRandom);
  uniRandom.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 1; });

  auto& uniDetune = result->params[(int)FFOsciParam::UniDetune];
  uniDetune.acc = true;
  uniDetune.defaultText = "33";
  uniDetune.display = "Dtn";
  uniDetune.name = "Unison Detune";
  uniDetune.slotCount = 1;
  uniDetune.unit = "%";
  uniDetune.id = "{C73A2DFD-0C6D-47FF-A524-CA14A75DF418}";
  uniDetune.type = FBParamType::Identity;
  auto selectUniDetune = [](auto& module) { return &module.acc.uniDetune; };
  uniDetune.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniDetune);
  uniDetune.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniDetune);
  uniDetune.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniDetune);
  uniDetune.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 1; });

  auto& uniSpread = result->params[(int)FFOsciParam::UniSpread];
  uniSpread.acc = true;
  uniSpread.defaultText = "50";
  uniSpread.display = "Sprd";
  uniSpread.name = "Unison Spread";
  uniSpread.slotCount = 1;
  uniSpread.unit = "%";
  uniSpread.id = "{9F71BAA5-00A2-408B-8CFC-B70D84A7654E}";
  uniSpread.type = FBParamType::Identity;
  auto selectUniSpread = [](auto& module) { return &module.acc.uniSpread; };
  uniSpread.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniSpread);
  uniSpread.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniSpread);
  uniSpread.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniSpread);
  uniSpread.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 1; });

  auto& uniBlend = result->params[(int)FFOsciParam::UniBlend];
  uniBlend.acc = true;
  uniBlend.defaultText = "100";
  uniBlend.display = "Blnd";
  uniBlend.name = "Unison Blend";
  uniBlend.slotCount = 1;
  uniBlend.unit = "%";
  uniBlend.id = "{68974AC4-57ED-41E4-9B0F-6DB29E0B6BBB}";
  uniBlend.type = FBParamType::Identity;
  auto selectUniBlend = [](auto& module) { return &module.acc.uniBlend; };
  uniBlend.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniBlend);
  uniBlend.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniBlend);
  uniBlend.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniBlend);
  uniBlend.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 1; });

  auto& waveBasicMode = result->params[(int)FFOsciParam::WaveBasicMode];
  waveBasicMode.acc = false;
  waveBasicMode.name = "Basic Mode";
  waveBasicMode.slotCount = FFOsciWaveBasicCount;
  waveBasicMode.id = "{296806B7-DEC4-47F5-AEE0-C35B119CF871}";
  waveBasicMode.type = FBParamType::List;
  waveBasicMode.defaultTextSelector = [](int /*ms*/, int ps) { return ps == 0 ? "Saw" : "Off"; };
  waveBasicMode.List().items = {
    { "{00880EBC-8E91-44C6-ADD4-4D2BB9B4E945}", "Off" },
    { "{FE9687FE-2A25-4FD3-8138-D775AC0103C6}", "Sin" },
    { "{6A17D1AC-C7EB-46DF-B05B-02F4AB34F402}", "Cos" },
    { "{0D1AA9D3-4CA5-4B67-BFFD-5D5F2C5B7161}", "Saw" },
    { "{8B420C8F-B2F7-4498-8481-678D3DBD10EF}", "Tri" },
    { "{CCA18B27-785F-4F33-BCE7-89C27CF9EEC2}", "Rect" },
    { "{C858202B-8FD4-4C53-93D4-AB2FF019068C}", "Ramp" },
    { "{B559ECEF-AEF2-448D-A2E8-87C76B42C110}", "SinSqr" },
    { "{67B63472-31C1-4C72-AE37-D377FB342E7C}", "SawSqr" },
    { "{AB878F10-8D9A-4C21-AA6D-E18055EAEB9E}", "SinSaw" },
    { "{884C404C-9831-4A36-8669-C0281C334DCF}", "BSSin" },
    { "{D96284EC-DB70-4C30-9F40-CC9789C10211}", "HWSin" },
    { "{AA6D107C-A4D6-41D2-AA4F-DA05923B2555}", "FWSin" },
    { "{9497D99D-5458-4D26-9633-F2A26B987681}", "AltSin" },
    { "{555A6AC2-2F65-438B-BAE3-7151136EC983}", "Trap" },
    { "{A3FA17C6-E9F7-4BA6-B791-FF847B3A1B0F}", "SawM1" },
    { "{40DF0782-F181-4956-A665-29BAD08E0D6F}", "SqrM1" },
    { "{EE5AC911-DB72-4D22-ABD0-EBBE72141394}", "Parabl" },
    { "{B3BC94E7-226A-483B-B406-C2879EF62560}", "HypTri" }
  };
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::Off] = "Off";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::Sin] = "Classic";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::SinSqr] = "Mixed";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::BSSin] = "ModSin";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::Trap] = "Other";
  auto selectWaveBasicMode = [](auto& module) { return &module.block.waveBasicMode; };
  waveBasicMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveBasicMode);
  waveBasicMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectWaveBasicMode);
  waveBasicMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveBasicMode);
  waveBasicMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave; });

  auto& waveBasicGain = result->params[(int)FFOsciParam::WaveBasicGain];
  waveBasicGain.acc = true;
  waveBasicGain.defaultText = "100";
  waveBasicGain.name = "Basic Gain";
  waveBasicGain.display = "Gn";
  waveBasicGain.slotCount = FFOsciWaveBasicCount;
  waveBasicGain.unit = "%";
  waveBasicGain.id = "{9B04E634-D046-4117-A542-7E050F3B5FB5}";
  waveBasicGain.type = FBParamType::Linear;
  waveBasicGain.Linear().min = -1.0f;
  waveBasicGain.Linear().max = 1.0f;
  waveBasicGain.Linear().displayMultiplier = 100.0f;
  auto selectWaveBasicGain = [](auto& module) { return &module.acc.waveBasicGain; };
  waveBasicGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveBasicGain);
  waveBasicGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectWaveBasicGain);
  waveBasicGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveBasicGain);
  waveBasicGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveBasicMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& wavePWMode = result->params[(int)FFOsciParam::WavePWMode];
  wavePWMode.acc = false;
  wavePWMode.defaultText = "PW Off";
  wavePWMode.name = "PW Mode";
  wavePWMode.slotCount = FFOsciWavePWCount;
  wavePWMode.id = "{E4159ACA-C4A9-4430-8E4A-44EB5DB8557A}";
  wavePWMode.type = FBParamType::List;
  wavePWMode.List().items = {
    { "{BAAB3335-9DF4-4D97-957A-6FBF47D5089E}", "PW Off" },
    { "{DFD55382-FBA0-4080-B179-98385452528B}", "PW Sqr" },
    { "{4C9F71AC-ECC2-4D07-8058-2D29FB967BF6}", "PW Rect" },
    { "{FAFD0A34-62D0-4A85-B450-BAEA8B5AA35C}", "PW TriPls" },
    { "{C69D964F-926E-4100-9558-2D43CCE01853}", "PW TriSaw" },
    { "{465F6A72-2EA2-4EB7-974E-600F5A724CE4}", "PW TrapTri" },
    { "{7DB51B2E-0C60-438C-B285-82D05855057F}", "PW HWSaw" },
  };
  auto selectWavePWMode = [](auto& module) { return &module.block.wavePWMode; };
  wavePWMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWavePWMode);
  wavePWMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectWavePWMode);
  wavePWMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWavePWMode);
  wavePWMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave; });

  auto& wavePWGain = result->params[(int)FFOsciParam::WavePWGain];
  wavePWGain.acc = true;
  wavePWGain.defaultText = "100";
  wavePWGain.name = "PW Gain";
  wavePWGain.display = "Gn";
  wavePWGain.slotCount = FFOsciWavePWCount;
  wavePWGain.unit = "%";
  wavePWGain.id = "{CB7B0BA4-2182-4EA8-9895-1763A29DD9F0}";
  wavePWGain.type = FBParamType::Linear;
  wavePWGain.Linear().min = -1.0f;
  wavePWGain.Linear().max = 1.0f;
  wavePWGain.Linear().displayMultiplier = 100.0f;
  auto selectWavePWGain = [](auto& module) { return &module.acc.wavePWGain; };
  wavePWGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWavePWGain);
  wavePWGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectWavePWGain);
  wavePWGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWavePWGain);
  wavePWGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WavePWMode }, 
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& wavePWPW = result->params[(int)FFOsciParam::WavePWPW];
  wavePWPW.acc = true;
  wavePWPW.defaultText = "100";
  wavePWPW.name = "PW Pulse Width";
  wavePWPW.display = "PW";
  wavePWPW.slotCount = FFOsciWavePWCount;
  wavePWPW.unit = "%";
  wavePWPW.id = "{17BF0368-AC81-45B5-87F3-95958A0C02B6}";
  wavePWPW.type = FBParamType::Identity;
  auto selectWavePWPW = [](auto& module) { return &module.acc.wavePWPW; };
  wavePWPW.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWavePWPW);
  wavePWPW.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectWavePWPW);
  wavePWPW.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWavePWPW);
  wavePWPW.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WavePWMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& waveHSMode = result->params[(int)FFOsciParam::WaveHSMode];
  waveHSMode.acc = false;
  waveHSMode.defaultText = "HS Off";
  waveHSMode.name = "HS Mode";
  waveHSMode.slotCount = 1;
  waveHSMode.id = "{F239E1E3-8889-4B36-B909-77205ACD00DA}";
  waveHSMode.type = FBParamType::List;
  waveHSMode.List().items = {
    { "{F68B6202-6C23-4049-B0DD-2565694B1C45}", "HS Off" },
    { "{2BBF8E97-0D2A-4F12-B608-47E241A5E125}", "HS Saw" },
    { "{4B99FF6B-9848-4F4E-8D97-0F0C3BA4F74C}", "HS Sqr" },
    { "{C7843450-2A2F-4FD0-A6C9-0B44F67F3170}", "HS Tri" }
  };
  auto selectWaveHSMode = [](auto& module) { return &module.block.waveHSMode; };
  waveHSMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveHSMode);
  waveHSMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectWaveHSMode);
  waveHSMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveHSMode);
  waveHSMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave; });

  auto& waveHSGain = result->params[(int)FFOsciParam::WaveHSGain];
  waveHSGain.acc = true;
  waveHSGain.defaultText = "100";
  waveHSGain.name = "HS Gain";
  waveHSGain.display = "Gn";
  waveHSGain.slotCount = 1;
  waveHSGain.unit = "%";
  waveHSGain.id = "{7AFF257F-165E-427B-A4BF-9DC4BFE48528}";
  waveHSGain.type = FBParamType::Linear;
  waveHSGain.Linear().min = -1.0f;
  waveHSGain.Linear().max = 1.0f;
  waveHSGain.Linear().displayMultiplier = 100.0f;
  auto selectWaveHSGain = [](auto& module) { return &module.acc.waveHSGain; };
  waveHSGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveHSGain);
  waveHSGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectWaveHSGain);
  waveHSGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveHSGain);
  waveHSGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveHSMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& waveHSSync = result->params[(int)FFOsciParam::WaveHSSync];
  waveHSSync.acc = true;
  waveHSSync.defaultText = "0";
  waveHSSync.name = "HS Sync Pitch";
  waveHSSync.display = "HSync";
  waveHSSync.slotCount = 1;
  waveHSSync.unit = "Semitones";
  waveHSSync.id = "{8551E49B-1D61-482D-8C2D-B766084C31D7}";
  waveHSSync.type = FBParamType::Linear;
  waveHSSync.Linear().min = 0.0f;
  waveHSSync.Linear().max = 48.0f;
  auto selectWaveHSSync = [](auto& module) { return &module.acc.waveHSSync; };
  waveHSSync.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveHSSync);
  waveHSSync.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectWaveHSSync);
  waveHSSync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveHSSync);
  waveHSSync.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveHSMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& waveDSFMode = result->params[(int)FFOsciParam::WaveDSFMode];
  waveDSFMode.acc = false;
  waveDSFMode.defaultText = "DSF Off";
  waveDSFMode.name = "DSF Mode";
  waveDSFMode.slotCount = 1;
  waveDSFMode.id = "{D66E2800-CFBD-4B4E-B22E-D5D7572FEF6E}";
  waveDSFMode.type = FBParamType::List;
  waveDSFMode.List().items = {
    { "{1CB6EE7D-3DAA-446F-82BF-4FADFD244EBE}", "DSF Off" },
    { "{738D0080-1F95-4FBD-AA50-DBA62CA25655}", "DSF Over" },
    { "{653EE5D8-D27D-4094-84EB-7FB6336F2DAB}", "DSF BW" } };
  auto selectWaveDSFMode = [](auto& module) { return &module.block.waveDSFMode; };
  waveDSFMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveDSFMode);
  waveDSFMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectWaveDSFMode);
  waveDSFMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveDSFMode);
  waveDSFMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave; });

  auto& waveDSFGain = result->params[(int)FFOsciParam::WaveDSFGain];
  waveDSFGain.acc = true;
  waveDSFGain.defaultText = "100";
  waveDSFGain.name = "DSF Gain";
  waveDSFGain.display = "Gn";
  waveDSFGain.slotCount = 1;
  waveDSFGain.unit = "%";
  waveDSFGain.id = "{8B92892C-4B53-4628-8266-CCED37867F99}";
  waveDSFGain.type = FBParamType::Linear;
  waveDSFGain.Linear().min = -1.0f;
  waveDSFGain.Linear().max = 1.0f;
  waveDSFGain.Linear().displayMultiplier = 100.0f;
  auto selectWaveDSFGain = [](auto& module) { return &module.acc.waveDSFGain; };
  waveDSFGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveDSFGain);
  waveDSFGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectWaveDSFGain);
  waveDSFGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveDSFGain);
  waveDSFGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveDSFMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& waveDSFOver = result->params[(int)FFOsciParam::WaveDSFOver];
  waveDSFOver.acc = false;
  waveDSFOver.defaultText = "1";
  waveDSFOver.name = "DSF Overtones";
  waveDSFOver.display = "Over";
  waveDSFOver.slotCount = 1;
  waveDSFOver.id = "{9A42FADE-5E48-49B8-804B-0C61E17AC3BB}";
  waveDSFOver.type = FBParamType::Discrete;
  waveDSFOver.Discrete().valueCount = 32;
  waveDSFOver.Discrete().valueOffset = 1;
  auto selectWaveDSFOver = [](auto& module) { return &module.block.waveDSFOver; };
  waveDSFOver.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveDSFOver);
  waveDSFOver.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectWaveDSFOver);
  waveDSFOver.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveDSFOver);
  waveDSFOver.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveDSFMode },
    [](auto const& vs) { return vs[0] != (int)FFOsciType::FM && vs[1] != (int)FFOsciWaveDSFMode::BW; });
  waveDSFOver.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveDSFMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] == (int)FFOsciWaveDSFMode::Over; });

  auto& waveDSFBW = result->params[(int)FFOsciParam::WaveDSFBW];
  waveDSFBW.acc = false;
  waveDSFBW.defaultText = "50";
  waveDSFBW.name = "DSF Bandwidth";
  waveDSFBW.display = "BW";
  waveDSFBW.slotCount = 1;
  waveDSFBW.unit = "%";
  waveDSFBW.id = "{D3D24159-2A4F-46FB-8E61-749DB07FCC40}";
  waveDSFBW.type = FBParamType::Log2;
  waveDSFBW.Log2().displayMultiplier = 100.0f;
  waveDSFBW.Log2().Init(-0.01f, 0.01f, 1.01f);
  auto selectWaveDSFBW = [](auto& module) { return &module.block.waveDSFBW; };
  waveDSFBW.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveDSFBW);
  waveDSFBW.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectWaveDSFBW);
  waveDSFBW.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveDSFBW);
  waveDSFBW.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveDSFMode },
    [](auto const& vs) { return vs[0] != (int)FFOsciType::FM && vs[1] == (int)FFOsciWaveDSFMode::BW; });
  waveDSFBW.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveDSFMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] == (int)FFOsciWaveDSFMode::BW; });

  auto& waveDSFDistance = result->params[(int)FFOsciParam::WaveDSFDistance];
  waveDSFDistance.acc = false;
  waveDSFDistance.defaultText = "1";
  waveDSFDistance.name = "DSF Distance";
  waveDSFDistance.display = "Dist";
  waveDSFDistance.slotCount = 1;
  waveDSFDistance.id = "{0D1D4920-A17F-4716-A42E-238DD1E99952}";
  waveDSFDistance.type = FBParamType::Discrete;
  waveDSFDistance.Discrete().valueCount = 20;
  waveDSFDistance.Discrete().valueOffset = 1;
  auto selectWaveDSFDistance = [](auto& module) { return &module.block.waveDSFDistance; };
  waveDSFDistance.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveDSFDistance);
  waveDSFDistance.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectWaveDSFDistance);
  waveDSFDistance.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveDSFDistance);
  waveDSFDistance.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveDSFMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });
  
  auto& waveDSFDecay = result->params[(int)FFOsciParam::WaveDSFDecay];
  waveDSFDecay.acc = true;
  waveDSFDecay.defaultText = "50";
  waveDSFDecay.name = "DSF Decay";
  waveDSFDecay.display = "Dcy";
  waveDSFDecay.slotCount = 1;
  waveDSFDecay.unit = "%";
  waveDSFDecay.id = "{21CE915D-0983-4545-9F6E-8743CAC5EAB7}";
  waveDSFDecay.type = FBParamType::Identity;
  auto selectWaveDSFDecay = [](auto& module) { return &module.acc.waveDSFDecay; };
  waveDSFDecay.scalarAddr = FFSelectScalarParamAddr(selectModule, selectWaveDSFDecay);
  waveDSFDecay.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectWaveDSFDecay);
  waveDSFDecay.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectWaveDSFDecay);
  waveDSFDecay.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveDSFMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& fmMode = result->params[(int)FFOsciParam::FMMode];
  fmMode.acc = false;
  fmMode.defaultText = "Linear";
  fmMode.name = "FM Mode";
  fmMode.display = "Mode";
  fmMode.slotCount = 1;
  fmMode.id = "{BE60503A-3CE3-422D-8795-C2FCB1C4A3B6}";
  fmMode.type = FBParamType::List;
  fmMode.List().items = {
    { "{8DF1F983-F892-4B5A-B784-C4222563E5BC}", "Linear" },
    { "{5DE278F5-21B2-4DD5-AB5A-BBF43BDFDD7F}", "Exp" } };
  auto selectFMMode = [](auto& module) { return &module.block.fmMode; };
  fmMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFMMode);
  fmMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFMMode);
  fmMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFMMode);
  fmMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

  auto& fmRatioMode = result->params[(int)FFOsciParam::FMRatioMode];
  fmRatioMode.acc = false;
  fmRatioMode.defaultText = "Ratio";
  fmRatioMode.name = "FM Ratio Mode";
  fmRatioMode.display = "Ratio";
  fmRatioMode.slotCount = 1;
  fmRatioMode.id = "{40838341-6882-4BF8-813F-BA5B89B3042F}";
  fmRatioMode.type = FBParamType::List;
  fmRatioMode.List().items = {
    { "{A6E83AA7-9DA3-4ECA-90DE-BCA123B48203}", "Ratio" },
    { "{B4AF1436-6ED3-4B43-A6F0-F5B624E4121D}", "Free" } };
  auto selectFMRatioMode = [](auto& module) { return &module.block.fmRatioMode; };
  fmRatioMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFMRatioMode);
  fmRatioMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFMRatioMode);
  fmRatioMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFMRatioMode);
  fmRatioMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

  auto& fmRatioFree = result->params[(int)FFOsciParam::FMRatioFree];
  fmRatioFree.acc = true;
  fmRatioFree.defaultText = "1";
  fmRatioFree.name = "FM Ratio Free";
  fmRatioFree.slotCount = FFOsciFMOperatorCount - 1;
  fmRatioFree.id = "{0188A986-8FA9-4BA2-BF84-A1A463712A40}";
  fmRatioFree.type = FBParamType::Log2;
  fmRatioFree.Log2().Init(0.0f, 1.0f / FFOsciFMRatioCount, FFOsciFMRatioCount);
  fmRatioFree.slotFormatter = FFOsciFMFormatRatioSlot;
  auto selectFMRatioFree = [](auto& module) { return &module.acc.fmRatioFree; };
  fmRatioFree.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::FMRatioMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM && vs[1] == (int)FFOsciFMRatioMode::Free; });

  auto& fmRatioRatio = result->params[(int)FFOsciParam::FMRatioRatio];
  fmRatioRatio.acc = false;
  fmRatioRatio.defaultText = "1:1";
  fmRatioRatio.name = "FM Ratio Ratio";
  fmRatioRatio.slotCount = FFOsciFMOperatorCount - 1;
  fmRatioRatio.id = "{9F79D937-E295-41FC-ACFE-F085E12DFF90}";
  fmRatioRatio.type = FBParamType::Discrete;
  fmRatioRatio.slotFormatter = FFOsciFMFormatRatioSlot;
  fmRatioRatio.Discrete().subMenuItemCount = FFOsciFMRatioCount;
  fmRatioRatio.Discrete().valueCount = FFOsciFMRatioCount * FFOsciFMRatioCount;
  fmRatioRatio.Discrete().valueFormatter = FFOsciFMFormatRatioValue;
  fmRatioRatio.Discrete().subMenuFormatter = FFOsciFMFormatRatioSubMenu;
  auto selectFMRatioRatio = [](auto& module) { return &module.block.fmRatioRatio; };
  fmRatioRatio.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFMRatioRatio);
  fmRatioRatio.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFMRatioRatio);
  fmRatioRatio.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFMRatioRatio);
  fmRatioRatio.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::FMRatioMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM && vs[1] == (int)FFOsciFMRatioMode::Ratio; });

  auto& fmIndex = result->params[(int)FFOsciParam::FMIndex];
  fmIndex.acc = true;
  fmIndex.defaultText = "0";
  fmIndex.name = "FM Index";
  fmIndex.slotCount = FFOsciFMMatrixSize;
  fmIndex.id = "{5CEFAD50-CB71-4E79-B3D6-50B004AD7F03}";
  fmIndex.type = FBParamType::Log2;
  fmIndex.Log2().Init(-0.01, 0.01f, 16.01f);
  fmIndex.slotFormatter = FFOsciFMFormatIndexSlot;
  auto selectFMIndex = [](auto& module) { return &module.acc.fmIndex; };
  fmIndex.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFMIndex);
  fmIndex.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectFMIndex);
  fmIndex.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFMIndex);
  fmIndex.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

  auto& stringMode = result->params[(int)FFOsciParam::StringMode];
  stringMode.acc = false;
  stringMode.defaultText = "Uni";
  stringMode.name = "String Mode";
  stringMode.slotCount = 1;
  stringMode.id = "{83BC3F01-EF0B-4BAE-AA95-012E246C87B3}";
  stringMode.type = FBParamType::List;
  stringMode.List().items = {
    { "{604E32C2-B3E9-4547-88B0-601F0D3AD055}", "Uni" },
    { "{B462657B-B0FB-47B1-AACA-0DF284E655E9}", "Norm" } };
  auto selectStringMode = [](auto& module) { return &module.block.stringMode; };
  stringMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringMode);
  stringMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStringMode);
  stringMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringMode);
  stringMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringSeed = result->params[(int)FFOsciParam::StringSeed];
  stringSeed.acc = false;
  stringSeed.defaultText = "0";
  stringSeed.name = "Seed";
  stringSeed.slotCount = 1;
  stringSeed.id = "{D5715C38-3695-4572-B7C9-8B150FDC4EA5}";
  stringSeed.type = FBParamType::Discrete;
  stringSeed.Discrete().valueCount = FFOsciStringMaxSeed + 1;
  auto selectStringSeed = [](auto& module) { return &module.block.stringSeed; };
  stringSeed.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringSeed);
  stringSeed.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStringSeed);
  stringSeed.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringSeed);
  stringSeed.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringPoles = result->params[(int)FFOsciParam::StringPoles];
  stringPoles.acc = false;
  stringPoles.defaultText = "4";
  stringPoles.name = "Quality";
  stringPoles.display = "Qlty";
  stringPoles.slotCount = 1;
  stringPoles.id = "{D3DFC350-647B-4492-A783-1373780023C6}";
  stringPoles.type = FBParamType::Discrete;
  stringPoles.Discrete().valueOffset = 1;
  stringPoles.Discrete().valueCount = FFOsciStringMaxPoles;
  auto selectStringPoles = [](auto& module) { return &module.block.stringPoles; };
  stringPoles.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringPoles);
  stringPoles.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStringPoles);
  stringPoles.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringPoles);
  stringPoles.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringColor = result->params[(int)FFOsciParam::StringColor];
  stringColor.acc = true;
  stringColor.defaultText = "0";
  stringColor.name = "Color";
  stringColor.slotCount = 1;
  stringColor.unit = "%";
  stringColor.id = "{FB9AC808-8A86-45A9-8A4E-E7E1B3A8D112}";
  stringColor.type = FBParamType::Identity;
  auto selectStringColor = [](auto& module) { return &module.acc.stringColor; };
  stringColor.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringColor);
  stringColor.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringColor);
  stringColor.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringColor);
  stringColor.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringX = result->params[(int)FFOsciParam::StringX];
  stringX.acc = true;
  stringX.defaultText = "100";
  stringX.name = "X";
  stringX.slotCount = 1;
  stringX.unit = "%";
  stringX.id = "{1000958E-9D9E-475B-8EB1-246939A378C9}";
  stringX.type = FBParamType::Identity;
  auto selectStringX = [](auto& module) { return &module.acc.stringX; };
  stringX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringX);
  stringX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringX);
  stringX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringX);
  stringX.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringY = result->params[(int)FFOsciParam::StringY];
  stringY.acc = true;
  stringY.defaultText = "100";
  stringY.name = "Y";
  stringY.slotCount = 1;
  stringY.unit = "%";
  stringY.id = "{1443614F-CE58-4666-BCD4-DE0F349AFB3E}";
  stringY.type = FBParamType::Identity;
  auto selectStringY = [](auto& module) { return &module.acc.stringY; };
  stringY.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringY);
  stringY.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringY);
  stringY.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringY);
  stringY.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringExcite = result->params[(int)FFOsciParam::StringExcite];
  stringExcite.acc = true;
  stringExcite.defaultText = "0";
  stringExcite.name = "Excite";
  stringExcite.slotCount = 1;
  stringExcite.unit = "%";
  stringExcite.id = "{02590DDB-B5B2-4FA6-94C5-8D0319450689}";
  stringExcite.type = FBParamType::Log2;
  stringExcite.Log2().displayMultiplier = 100.0f;
  stringExcite.Log2().Init(-0.01f, 0.01f, 1.01f);
  auto selectStringExcite = [](auto& module) { return &module.acc.stringExcite; };
  stringExcite.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringExcite);
  stringExcite.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringExcite);
  stringExcite.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringExcite);
  stringExcite.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringLPOn = result->params[(int)FFOsciParam::StringLPOn];
  stringLPOn.acc = false;
  stringLPOn.name = "LP On";
  stringLPOn.display = "LP";
  stringLPOn.slotCount = 1;
  stringLPOn.defaultText = "Off";
  stringLPOn.id = "{243E497E-2449-49BC-AA26-418743265570}";
  stringLPOn.type = FBParamType::Boolean;
  auto selectStringLPOn = [](auto& module) { return &module.block.stringLPOn; };
  stringLPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringLPOn);
  stringLPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStringLPOn);
  stringLPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringLPOn);
  stringLPOn.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringLPFreq = result->params[(int)FFOsciParam::StringLPFreq];
  stringLPFreq.acc = true;
  stringLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  stringLPFreq.name = "LP Freq";
  stringLPFreq.display = "LP";
  stringLPFreq.slotCount = 1;
  stringLPFreq.unit = "Hz";
  stringLPFreq.id = "{F8865388-AD37-4A9F-92DC-9AAB62BCF04E}";
  stringLPFreq.type = FBParamType::Log2;
  stringLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectStringLPFreq = [](auto& module) { return &module.acc.stringLPFreq; };
  stringLPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringLPFreq);
  stringLPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringLPFreq);
  stringLPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringLPFreq);
  stringLPFreq.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::StringLPOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String && vs[1] != 0; });

  auto& stringLPRes = result->params[(int)FFOsciParam::StringLPRes];
  stringLPRes.acc = true;
  stringLPRes.defaultText = "0";
  stringLPRes.name = "LP Res";
  stringLPRes.display = "Res";
  stringLPRes.slotCount = 1;
  stringLPRes.unit = "%";
  stringLPRes.id = "{A0FF6017-BF7D-446C-91E6-8893A696D2BA}";
  stringLPRes.type = FBParamType::Identity;
  auto selectStringLPRes = [](auto& module) { return &module.acc.stringLPRes; };
  stringLPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringLPRes);
  stringLPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringLPRes);
  stringLPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringLPRes);
  stringLPRes.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::StringLPOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String && vs[1] != 0; });

  auto& stringLPKTrk = result->params[(int)FFOsciParam::StringLPKTrk];
  stringLPKTrk.acc = true;
  stringLPKTrk.defaultText = "0";
  stringLPKTrk.name = "LP KeyTrk";
  stringLPKTrk.display = "KTrk";
  stringLPKTrk.slotCount = 1;
  stringLPKTrk.unit = "%";
  stringLPKTrk.id = "{C6EFCD36-256A-4F2C-B772-D5966460E893}";
  stringLPKTrk.type = FBParamType::Linear;
  stringLPKTrk.Linear().min = -2.0f;
  stringLPKTrk.Linear().max = 2.0f;
  stringLPKTrk.Linear().displayMultiplier = 100;
  auto selectStringLPKeyTrk = [](auto& module) { return &module.acc.stringLPKTrk; };
  stringLPKTrk.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringLPKeyTrk);
  stringLPKTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringLPKeyTrk);
  stringLPKTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringLPKeyTrk);
  stringLPKTrk.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::StringLPOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String && vs[1] != 0; });

  auto& stringHPOn = result->params[(int)FFOsciParam::StringHPOn];
  stringHPOn.acc = false;
  stringHPOn.name = "HP On";
  stringHPOn.display = "HP";
  stringHPOn.slotCount = 1;
  stringHPOn.defaultText = "Off";
  stringHPOn.id = "{F03C6E50-01BB-4C61-9122-C6599C9D4CBA}";
  stringHPOn.type = FBParamType::Boolean;
  auto selectStringHPOn = [](auto& module) { return &module.block.stringHPOn; };
  stringHPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringHPOn);
  stringHPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStringHPOn);
  stringHPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringHPOn);
  stringHPOn.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringHPFreq = result->params[(int)FFOsciParam::StringHPFreq];
  stringHPFreq.acc = true;
  stringHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  stringHPFreq.name = "HP";
  stringHPFreq.slotCount = 1;
  stringHPFreq.unit = "Hz";
  stringHPFreq.id = "{1753A4C9-BE63-4079-A875-59C35F3BC584}";
  stringHPFreq.type = FBParamType::Log2;
  stringHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectStringHPFreq = [](auto& module) { return &module.acc.stringHPFreq; };
  stringHPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringHPFreq);
  stringHPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringHPFreq);
  stringHPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringHPFreq);
  stringHPFreq.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::StringHPOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String && vs[1] != 0; });

  auto& stringHPRes = result->params[(int)FFOsciParam::StringHPRes];
  stringHPRes.acc = true;
  stringHPRes.defaultText = "0";
  stringHPRes.name = "HP Res";
  stringHPRes.display = "Res";
  stringHPRes.slotCount = 1;
  stringHPRes.unit = "%";
  stringHPRes.id = "{164AD99E-1C52-4302-8032-4E02F7A43224}";
  stringHPRes.type = FBParamType::Identity;
  auto selectStringHPRes = [](auto& module) { return &module.acc.stringHPRes; };
  stringHPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringHPRes);
  stringHPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringHPRes);
  stringHPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringHPRes);
  stringHPRes.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::StringHPOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String && vs[1] != 0; });

  auto& stringHPKTrk = result->params[(int)FFOsciParam::StringHPKTrk];
  stringHPKTrk.acc = true;
  stringHPKTrk.defaultText = "0";
  stringHPKTrk.name = "HP KeyTrk";
  stringHPKTrk.display = "KTrk";
  stringHPKTrk.slotCount = 1;
  stringHPKTrk.unit = "%";
  stringHPKTrk.id = "{61BA0D46-9BC3-496C-A004-671F3465142E}";
  stringHPKTrk.type = FBParamType::Linear;
  stringHPKTrk.Linear().min = -2.0f;
  stringHPKTrk.Linear().max = 2.0f;
  stringHPKTrk.Linear().displayMultiplier = 100;
  auto selectStringHPKeyTrk = [](auto& module) { return &module.acc.stringHPKTrk; };
  stringHPKTrk.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringHPKeyTrk);
  stringHPKTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringHPKeyTrk);
  stringHPKTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringHPKeyTrk);
  stringHPKTrk.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::StringHPOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String && vs[1] != 0; });

  auto& stringDamp = result->params[(int)FFOsciParam::StringDamp];
  stringDamp.acc = true;
  stringDamp.defaultText = "67";
  stringDamp.name = "Damp";
  stringDamp.slotCount = 1;
  stringDamp.unit = "%";
  stringDamp.id = "{50FA6C2A-64FC-4B2B-BC64-55A8EA7472F4}";
  stringDamp.type = FBParamType::Identity;
  auto selectStringDamp = [](auto& module) { return &module.acc.stringDamp; };
  stringDamp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringDamp);
  stringDamp.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringDamp);
  stringDamp.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringDamp);
  stringDamp.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringDampKTrk = result->params[(int)FFOsciParam::StringDampKTrk];
  stringDampKTrk.acc = true;
  stringDampKTrk.defaultText = "0";
  stringDampKTrk.name = "Damp KTrk";
  stringDampKTrk.display = "KTrk";
  stringDampKTrk.slotCount = 1;
  stringDampKTrk.unit = "%";
  stringDampKTrk.id = "{5B4F67F9-30E9-482C-922A-33F5CB7F5A1F}";
  stringDampKTrk.type = FBParamType::Linear;
  stringDampKTrk.Linear().min = -1.0f;
  stringDampKTrk.Linear().max = 1.0f;
  stringDampKTrk.Linear().displayMultiplier = 100;
  auto selectStringDampKTrk = [](auto& module) { return &module.acc.stringDampKTrk; };
  stringDampKTrk.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringDampKTrk);
  stringDampKTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringDampKTrk);
  stringDampKTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringDampKTrk);
  stringDampKTrk.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringFeedback = result->params[(int)FFOsciParam::StringFeedback];
  stringFeedback.acc = true;
  stringFeedback.defaultText = "100";
  stringFeedback.name = "Feedback";
  stringFeedback.display = "Fdbk";
  stringFeedback.slotCount = 1;
  stringFeedback.unit = "%";
  stringFeedback.id = "{280B9667-8DA5-4DD6-B7CD-695DF38AA857}";
  stringFeedback.type = FBParamType::Identity;
  auto selectStringFeedback = [](auto& module) { return &module.acc.stringFeedback; };
  stringFeedback.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringFeedback);
  stringFeedback.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringFeedback);
  stringFeedback.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringFeedback);
  stringFeedback.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringFeedbackKTrk = result->params[(int)FFOsciParam::StringFeedbackKTrk];
  stringFeedbackKTrk.acc = true;
  stringFeedbackKTrk.defaultText = "0";
  stringFeedbackKTrk.name = "Feedback KTrk";
  stringFeedbackKTrk.display = "KTrk";
  stringFeedbackKTrk.slotCount = 1;
  stringFeedbackKTrk.unit = "%";
  stringFeedbackKTrk.id = "{239389B7-52BC-437F-909C-184621F69E79}";
  stringFeedbackKTrk.type = FBParamType::Linear;
  stringFeedbackKTrk.Linear().min = -1.0f;
  stringFeedbackKTrk.Linear().max = 1.0f;
  stringFeedbackKTrk.Linear().displayMultiplier = 100;
  auto selectStringFeedbackKTrk = [](auto& module) { return &module.acc.stringFeedbackKTrk; };
  stringFeedbackKTrk.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringFeedbackKTrk);
  stringFeedbackKTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringFeedbackKTrk);
  stringFeedbackKTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringFeedbackKTrk);
  stringFeedbackKTrk.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringTrackingKey = result->params[(int)FFOsciParam::StringTrackingKey];
  stringTrackingKey.acc = true;
  stringTrackingKey.defaultText = "0";
  stringTrackingKey.name = "Tracking Key";
  stringTrackingKey.display = "Key";
  stringTrackingKey.slotCount = 1;
  stringTrackingKey.unit = "Semitones";
  stringTrackingKey.id = "{469BF707-3F08-491B-95ED-F0C8DE75F8EA}";
  stringTrackingKey.type = FBParamType::Linear;
  stringTrackingKey.Linear().min = -64.0f;
  stringTrackingKey.Linear().max = 64.0f;
  auto selectStringTrackingKey = [](auto& module) { return &module.acc.stringTrackingKey; };
  stringTrackingKey.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringTrackingKey);
  stringTrackingKey.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringTrackingKey);
  stringTrackingKey.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringTrackingKey);
  stringTrackingKey.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  auto& stringTrackingRange = result->params[(int)FFOsciParam::StringTrackingRange];
  stringTrackingRange.acc = true;
  stringTrackingRange.defaultText = "24";
  stringTrackingRange.name = "Tracking Range";
  stringTrackingRange.display = "Range";
  stringTrackingRange.slotCount = 1;
  stringTrackingRange.unit = "Semitones";
  stringTrackingRange.id = "{31FFFDCC-6A5A-4D46-8209-C9C4F2540870}";
  stringTrackingRange.type = FBParamType::Linear;
  stringTrackingRange.Linear().min = 12.0f;
  stringTrackingRange.Linear().max = 128.0f;
  auto selectStringTrackingRange = [](auto& module) { return &module.acc.stringTrackingRange; };
  stringTrackingRange.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringTrackingRange);
  stringTrackingRange.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringTrackingRange);
  stringTrackingRange.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringTrackingRange);
  stringTrackingRange.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::String; });

  return result;
}