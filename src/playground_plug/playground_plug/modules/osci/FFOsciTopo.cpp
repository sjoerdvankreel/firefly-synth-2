#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/oscis_graph/FFOscisGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

static std::string
FFOsciFMFormatRatioSlot(int slot)
{
  switch (slot)
  {
  case 0: return "2:1";
  case 1: return "3:2";
  default: assert(false); return "";
  }
}

static std::string
FFOsciFMFormatRatioSubMenu(int subMenu)
{
  assert(0 <= subMenu && subMenu < FFOsciFMRatioCount);
  return std::to_string(subMenu + 1);
}

static std::string
FFOsciFMFormatRatioValue(int val)
{
  assert(0 <= val && val < FFOsciFMRatioCount * FFOsciFMRatioCount);
  return std::to_string(val / FFOsciFMRatioCount + 1) + ":" +
    std::to_string(val % FFOsciFMRatioCount + 1);
}

static std::string
FFOsciFMFormatIndexSlot(int slot)
{
  assert(0 <= slot && slot < FFOsciFMMatrixSize);
  return std::to_string(slot / FFOsciFMOperatorCount + 1) + ">" + 
    std::to_string(slot % FFOsciFMOperatorCount + 1);
}

std::unique_ptr<FBStaticModule>
FFMakeOsciTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc";
  result->slotCount = FFOsciCount;
  result->graphCount = FFOsciCount;
  result->graphRenderer = FFOscisRenderGraph;
  result->id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  result->params.resize((int)FFOsciParam::Count);
  result->addrSelectors.voiceModuleExchange = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.osci; });
  auto selectModule = [](auto& state) { return &state.voice.osci; };

  auto& type = result->params[(int)FFOsciParam::Type];
  type.acc = false;
  type.defaultText = "Off";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{9018865F-7B05-4835-B541-95014C0C63E6}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{449E467A-2DC0-43B0-8487-57C4492F9FE2}", "Off" },
    { "{3F55D6D7-5BDF-4B7F-B1E0-2E59B96EA5C0}", "Wave" },
    { "{19945EB6-4676-492A-BC38-E586A6D3BF6F}", "DSF" } ,
    { "{83E9DBC4-5CBF-4C96-93EB-AB16C2E7C769}", "FM" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectType);
  type.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectType);
  type.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectType);

  auto& gain = result->params[(int)FFOsciParam::Gain];
  gain.acc = true;
  gain.defaultText = "100";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.unit = "%";
  gain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  gain.type = FBParamType::Identity;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectGain);
  gain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectGain);

  auto& coarse = result->params[(int)FFOsciParam::Coarse];
  coarse.acc = true;
  coarse.defaultText = "0";
  coarse.name = "Coarse";
  coarse.slotCount = 1;
  coarse.unit = "Semitones";
  coarse.id = "{E122CA2C-C1B1-47E5-A1BB-DEAC6A4030E0}";
  coarse.type = FBParamType::Linear;
  coarse.Linear().min = -12.0f;
  coarse.Linear().max = 12.0f;
  auto selectCoarse = [](auto& module) { return &module.acc.coarse; };
  coarse.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectCoarse);
  coarse.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectCoarse);
  coarse.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectCoarse);

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
  fine.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFine);
  fine.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFine);
  fine.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFine);

  auto& uniCount = result->params[(int)FFOsciParam::UniCount];
  uniCount.acc = false;
  uniCount.defaultText = "1";
  uniCount.name = "Unison";
  uniCount.tooltip = "Unison Count";
  uniCount.slotCount = 1;
  uniCount.id = "{60313673-95FE-4B6D-99A6-B628ACDE6D56}";
  uniCount.type = FBParamType::Discrete;
  uniCount.Discrete().valueCount = FFOsciUniMaxCount;
  uniCount.Discrete().valueOffset = 1;
  auto selectUniCount = [](auto& module) { return &module.block.uniCount; };
  uniCount.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUniCount);
  uniCount.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUniCount);
  uniCount.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUniCount);

  auto& uniOffset = result->params[(int)FFOsciParam::UniOffset];
  uniOffset.acc = false;
  uniOffset.defaultText = "50";
  uniOffset.name = "Offset";
  uniOffset.tooltip = "Unison Offset";
  uniOffset.slotCount = 1;
  uniOffset.unit = "%";
  uniOffset.id = "{6F5754E1-BDF4-4685-98FC-8C613128EE8D}";
  uniOffset.type = FBParamType::Identity;
  auto selectUniOffset = [](auto& module) { return &module.block.uniOffset; };
  uniOffset.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUniOffset);
  uniOffset.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUniOffset);
  uniOffset.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUniOffset);
  uniOffset.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& uniRandom = result->params[(int)FFOsciParam::UniRandom];
  uniRandom.acc = false;
  uniRandom.defaultText = "50";
  uniRandom.name = "Random";
  uniRandom.tooltip = "Unison Random";
  uniRandom.slotCount = 1;
  uniRandom.unit = "%";
  uniRandom.id = "{6F7F6D55-5740-44AB-8442-267A5730E2DA}";
  uniRandom.type = FBParamType::Identity;
  auto selectUniRandom = [](auto& module) { return &module.block.uniRandom; };
  uniRandom.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUniRandom);
  uniRandom.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUniRandom);
  uniRandom.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUniRandom);
  uniRandom.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& uniDetune = result->params[(int)FFOsciParam::UniDetune];
  uniDetune.acc = true;
  uniDetune.defaultText = "33";
  uniDetune.name = "Detune";
  uniDetune.tooltip = "Unison Detune";
  uniDetune.slotCount = 1;
  uniDetune.unit = "%";
  uniDetune.id = "{C73A2DFD-0C6D-47FF-A524-CA14A75DF418}";
  uniDetune.type = FBParamType::Identity;
  auto selectUniDetune = [](auto& module) { return &module.acc.uniDetune; };
  uniDetune.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUniDetune);
  uniDetune.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectUniDetune);
  uniDetune.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUniDetune);
  uniDetune.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& uniSpread = result->params[(int)FFOsciParam::UniSpread];
  uniSpread.acc = true;
  uniSpread.defaultText = "50";
  uniSpread.name = "Spread";
  uniSpread.tooltip = "Unison Spread";
  uniSpread.slotCount = 1;
  uniSpread.unit = "%";
  uniSpread.id = "{9F71BAA5-00A2-408B-8CFC-B70D84A7654E}";
  uniSpread.type = FBParamType::Identity;
  auto selectUniSpread = [](auto& module) { return &module.acc.uniSpread; };
  uniSpread.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUniSpread);
  uniSpread.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectUniSpread);
  uniSpread.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUniSpread);
  uniSpread.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& uniBlend = result->params[(int)FFOsciParam::UniBlend];
  uniBlend.acc = true;
  uniBlend.defaultText = "100";
  uniBlend.name = "Blend";
  uniBlend.tooltip = "Unison Blend";
  uniBlend.slotCount = 1;
  uniBlend.unit = "%";
  uniBlend.id = "{68974AC4-57ED-41E4-9B0F-6DB29E0B6BBB}";
  uniBlend.type = FBParamType::Identity;
  auto selectUniBlend = [](auto& module) { return &module.acc.uniBlend; };
  uniBlend.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUniBlend);
  uniBlend.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectUniBlend);
  uniBlend.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUniBlend);
  uniBlend.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& waveBasicMode = result->params[(int)FFOsciParam::WaveBasicMode];
  waveBasicMode.acc = false;
  waveBasicMode.defaultText = "Off";
  waveBasicMode.name = "Basic Mode";
  waveBasicMode.tooltip = "Basic Wave Mode";
  waveBasicMode.slotCount = FFOsciWaveBasicCount;
  waveBasicMode.id = "{296806B7-DEC4-47F5-AEE0-C35B119CF871}";
  waveBasicMode.type = FBParamType::List;
  waveBasicMode.List().items = {
    { "{00880EBC-8E91-44C6-ADD4-4D2BB9B4E945}", "Off" },
    { "{FE9687FE-2A25-4FD3-8138-D775AC0103C6}", "Sin", "Sine" },
    { "{6A17D1AC-C7EB-46DF-B05B-02F4AB34F402}", "Cos", "Cosine" },
    { "{0D1AA9D3-4CA5-4B67-BFFD-5D5F2C5B7161}", "Saw", "Saw" },
    { "{8B420C8F-B2F7-4498-8481-678D3DBD10EF}", "Tri", "Triangle" },
    { "{CCA18B27-785F-4F33-BCE7-89C27CF9EEC2}", "Rect", "Rect" },
    { "{C858202B-8FD4-4C53-93D4-AB2FF019068C}", "Ramp", "Ramp" },
    { "{B559ECEF-AEF2-448D-A2E8-87C76B42C110}", "SinSqr", "Sin Sqr" },
    { "{67B63472-31C1-4C72-AE37-D377FB342E7C}", "SawSqr", "Saw Sqr" },
    { "{AB878F10-8D9A-4C21-AA6D-E18055EAEB9E}", "SinSaw", "Sin Saw" },
    { "{884C404C-9831-4A36-8669-C0281C334DCF}", "BSSin", "Bipolar Sin^2" },
    { "{D96284EC-DB70-4C30-9F40-CC9789C10211}", "HWSin", "Half Rect Sin" },
    { "{AA6D107C-A4D6-41D2-AA4F-DA05923B2555}", "FWSin", "Full Rect Sin" },
    { "{9497D99D-5458-4D26-9633-F2A26B987681}", "AltSin", "Alternating Sin" },
    { "{555A6AC2-2F65-438B-BAE3-7151136EC983}", "Trap", "Trapezoid" },
    { "{A3FA17C6-E9F7-4BA6-B791-FF847B3A1B0F}", "SawM1", "Saw No Fundamental" },
    { "{40DF0782-F181-4956-A665-29BAD08E0D6F}", "SqrM1", "Sqr No Fundamental" },
    { "{EE5AC911-DB72-4D22-ABD0-EBBE72141394}", "Parabl", "Parabola" },
    { "{B3BC94E7-226A-483B-B406-C2879EF62560}", "HypTri", "Hyper Tri" }
  };
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::Off] = "Off";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::Sin] = "Classic";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::SinSqr] = "Mixed";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::BSSin] = "Mod Sin";
  waveBasicMode.List().submenuStart[(int)FFOsciWaveBasicMode::Trap] = "Other";
  auto selectWaveBasicMode = [](auto& module) { return &module.block.waveBasicMode; };
  waveBasicMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWaveBasicMode);
  waveBasicMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectWaveBasicMode);
  waveBasicMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWaveBasicMode);
  waveBasicMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave; });

  auto& waveBasicGain = result->params[(int)FFOsciParam::WaveBasicGain];
  waveBasicGain.acc = true;
  waveBasicGain.defaultText = "100";
  waveBasicGain.name = "Basic Gain";
  waveBasicGain.tooltip = "Basic Wave Gain";
  waveBasicGain.slotCount = FFOsciWaveBasicCount;
  waveBasicGain.unit = "%";
  waveBasicGain.id = "{9B04E634-D046-4117-A542-7E050F3B5FB5}";
  waveBasicGain.type = FBParamType::Linear;
  waveBasicGain.Linear().min = -1.0f;
  waveBasicGain.Linear().max = 1.0f;
  waveBasicGain.Linear().displayMultiplier = 100.0f;
  auto selectWaveBasicGain = [](auto& module) { return &module.acc.waveBasicGain; };
  waveBasicGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWaveBasicGain);
  waveBasicGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectWaveBasicGain);
  waveBasicGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWaveBasicGain);
  waveBasicGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveBasicMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& wavePWMode = result->params[(int)FFOsciParam::WavePWMode];
  wavePWMode.acc = false;
  wavePWMode.defaultText = "Off";
  wavePWMode.name = "PW Mode";
  wavePWMode.tooltip = "PW Wave Mode";
  wavePWMode.slotCount = FFOsciWavePWCount;
  wavePWMode.id = "{E4159ACA-C4A9-4430-8E4A-44EB5DB8557A}";
  wavePWMode.type = FBParamType::List;
  wavePWMode.List().items = {
    { "{BAAB3335-9DF4-4D97-957A-6FBF47D5089E}", "Off" },
    { "{DFD55382-FBA0-4080-B179-98385452528B}", "Sqr", "Sqr" },
    { "{4C9F71AC-ECC2-4D07-8058-2D29FB967BF6}", "Rect", "Rect" },
    { "{FAFD0A34-62D0-4A85-B450-BAEA8B5AA35C}", "TriPls", "Tri To Sqr" },
    { "{C69D964F-926E-4100-9558-2D43CCE01853}", "TriSaw", "Tri To Saw" },
    { "{465F6A72-2EA2-4EB7-974E-600F5A724CE4}", "TrapTri", "Trap To Tri" },
    { "{7DB51B2E-0C60-438C-B285-82D05855057F}", "HWSaw", "Half Rect Saw" },
  };
  auto selectWavePWMode = [](auto& module) { return &module.block.wavePWMode; };
  wavePWMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWavePWMode);
  wavePWMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectWavePWMode);
  wavePWMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWavePWMode);
  wavePWMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave; });

  auto& wavePWGain = result->params[(int)FFOsciParam::WavePWGain];
  wavePWGain.acc = true;
  wavePWGain.defaultText = "100";
  wavePWGain.name = "PW Gain";
  wavePWGain.tooltip = "PW Wave Gain";
  wavePWGain.slotCount = FFOsciWavePWCount;
  wavePWGain.unit = "%";
  wavePWGain.id = "{CB7B0BA4-2182-4EA8-9895-1763A29DD9F0}";
  wavePWGain.type = FBParamType::Linear;
  wavePWGain.Linear().min = -1.0f;
  wavePWGain.Linear().max = 1.0f;
  wavePWGain.Linear().displayMultiplier = 100.0f;
  auto selectWavePWGain = [](auto& module) { return &module.acc.wavePWGain; };
  wavePWGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWavePWGain);
  wavePWGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectWavePWGain);
  wavePWGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWavePWGain);
  wavePWGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WavePWMode }, 
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& wavePWPW = result->params[(int)FFOsciParam::WavePWPW];
  wavePWPW.acc = true;
  wavePWPW.defaultText = "100";
  wavePWPW.name = "Wave PW";
  wavePWPW.tooltip = "PW Wave PW";
  wavePWPW.slotCount = FFOsciWavePWCount;
  wavePWPW.unit = "%";
  wavePWPW.id = "{17BF0368-AC81-45B5-87F3-95958A0C02B6}";
  wavePWPW.type = FBParamType::Identity;
  auto selectWavePWPW = [](auto& module) { return &module.acc.wavePWPW; };
  wavePWPW.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWavePWPW);
  wavePWPW.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectWavePWPW);
  wavePWPW.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWavePWPW);
  wavePWPW.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WavePWMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& waveHSMode = result->params[(int)FFOsciParam::WaveHSMode];
  waveHSMode.acc = false;
  waveHSMode.defaultText = "Off";
  waveHSMode.name = "HS Mode";
  waveHSMode.tooltip = "HS Wave Mode";
  waveHSMode.slotCount = 1;
  waveHSMode.id = "{F239E1E3-8889-4B36-B909-77205ACD00DA}";
  waveHSMode.type = FBParamType::List;
  waveHSMode.List().items = {
    { "{F68B6202-6C23-4049-B0DD-2565694B1C45}", "Off" },
    { "{2BBF8E97-0D2A-4F12-B608-47E241A5E125}", "Saw", "Saw" },
    { "{4B99FF6B-9848-4F4E-8D97-0F0C3BA4F74C}", "Sqr", "Sqr" },
    { "{C7843450-2A2F-4FD0-A6C9-0B44F67F3170}", "Tri", "Tri" }
  };
  auto selectWaveHSMode = [](auto& module) { return &module.block.waveHSMode; };
  waveHSMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWaveHSMode);
  waveHSMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectWaveHSMode);
  waveHSMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWaveHSMode);
  waveHSMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave; });

  auto& waveHSGain = result->params[(int)FFOsciParam::WaveHSGain];
  waveHSGain.acc = true;
  waveHSGain.defaultText = "100";
  waveHSGain.name = "HS Gain";
  waveHSGain.tooltip = "HS Wave Gain";
  waveHSGain.slotCount = 1;
  waveHSGain.unit = "%";
  waveHSGain.id = "{7AFF257F-165E-427B-A4BF-9DC4BFE48528}";
  waveHSGain.type = FBParamType::Linear;
  waveHSGain.Linear().min = -1.0f;
  waveHSGain.Linear().max = 1.0f;
  waveHSGain.Linear().displayMultiplier = 100.0f;
  auto selectWaveHSGain = [](auto& module) { return &module.acc.waveHSGain; };
  waveHSGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWaveHSGain);
  waveHSGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectWaveHSGain);
  waveHSGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWaveHSGain);
  waveHSGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveHSMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& waveHSSync = result->params[(int)FFOsciParam::WaveHSSync];
  waveHSSync.acc = true;
  waveHSSync.defaultText = "0";
  waveHSSync.name = "Sync";
  waveHSSync.tooltip = "HS Sync";
  waveHSSync.slotCount = 1;
  waveHSSync.unit = "Semitones";
  waveHSSync.id = "{8551E49B-1D61-482D-8C2D-B766084C31D7}";
  waveHSSync.type = FBParamType::Linear;
  waveHSSync.Linear().min = 0.0f;
  waveHSSync.Linear().max = 48.0f;
  auto selectWaveHSSync = [](auto& module) { return &module.acc.waveHSSync; };
  waveHSSync.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectWaveHSSync);
  waveHSSync.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectWaveHSSync);
  waveHSSync.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectWaveHSSync);
  waveHSSync.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::WaveHSMode },
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Wave && vs[1] != 0; });

  auto& dsfMode = result->params[(int)FFOsciParam::DSFMode];
  dsfMode.acc = false;
  dsfMode.defaultText = "Overtones";
  dsfMode.name = "Mode";
  dsfMode.tooltip = "DSF Mode";     
  dsfMode.slotCount = 1;
  dsfMode.id = "{D66E2800-CFBD-4B4E-B22E-D5D7572FEF6E}";
  dsfMode.type = FBParamType::List;
  dsfMode.List().items = {
    { "{738D0080-1F95-4FBD-AA50-DBA62CA25655}", "Overtones" },
    { "{653EE5D8-D27D-4094-84EB-7FB6336F2DAB}", "Bandwidth" } };
  auto selectDSFMode = [](auto& module) { return &module.block.dsfMode; };
  dsfMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFMode);
  dsfMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDSFMode);
  dsfMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFMode);
  dsfMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF; });

  auto& dsfOvertones = result->params[(int)FFOsciParam::DSFOvertones];
  dsfOvertones.acc = false;
  dsfOvertones.defaultText = "1";
  dsfOvertones.name = "Overtones";
  dsfOvertones.tooltip = "DSF Overtones";
  dsfOvertones.slotCount = 1;
  dsfOvertones.id = "{9A42FADE-5E48-49B8-804B-0C61E17AC3BB}";
  dsfOvertones.type = FBParamType::Discrete;
  dsfOvertones.Discrete().valueCount = 32;
  dsfOvertones.Discrete().valueOffset = 1;
  auto selectDSFOvertones = [](auto& module) { return &module.block.dsfOvertones; };
  dsfOvertones.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFOvertones);
  dsfOvertones.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDSFOvertones);
  dsfOvertones.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFOvertones);
  dsfOvertones.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::DSFMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF && vs[1] == (int)FFOsciDSFMode::Overtones; });

  auto& dsfBandwidth = result->params[(int)FFOsciParam::DSFBandwidth];
  dsfBandwidth.acc = false;
  dsfBandwidth.defaultText = "50";
  dsfBandwidth.name = "Bandwidth";
  dsfBandwidth.tooltip = "DSF Bandwidth";
  dsfBandwidth.slotCount = 1;
  dsfBandwidth.unit = "%";
  dsfBandwidth.id = "{D3D24159-2A4F-46FB-8E61-749DB07FCC40}";
  dsfBandwidth.type = FBParamType::Log2;
  dsfBandwidth.Log2().Init(-1.0f, 1.0f, 101.0f);
  auto selectDSFBandwidth = [](auto& module) { return &module.block.dsfBandwidth; };
  dsfBandwidth.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFBandwidth);
  dsfBandwidth.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDSFBandwidth);
  dsfBandwidth.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFBandwidth);
  dsfBandwidth.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::DSFMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF && vs[1] == (int)FFOsciDSFMode::Bandwidth; });

  auto& dsfDistance = result->params[(int)FFOsciParam::DSFDistance];
  dsfDistance.acc = false;
  dsfDistance.defaultText = "1";
  dsfDistance.name = "Distance";
  dsfDistance.tooltip = "DSF Distance";
  dsfDistance.slotCount = 1;
  dsfDistance.id = "{0D1D4920-A17F-4716-A42E-238DD1E99952}";
  dsfDistance.type = FBParamType::Discrete;
  dsfDistance.Discrete().valueCount = 20;
  dsfDistance.Discrete().valueOffset = 1;
  auto selectDSFDistance = [](auto& module) { return &module.block.dsfDistance; };
  dsfDistance.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFDistance);
  dsfDistance.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDSFDistance);
  dsfDistance.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFDistance);
  dsfDistance.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF; });
  
  auto& dsfDecay = result->params[(int)FFOsciParam::DSFDecay];
  dsfDecay.acc = true;
  dsfDecay.defaultText = "50";
  dsfDecay.name = "Decay";
  dsfDecay.tooltip = "DSF Decay";
  dsfDecay.slotCount = 1;
  dsfDecay.unit = "%";
  dsfDecay.id = "{21CE915D-0983-4545-9F6E-8743CAC5EAB7}";
  dsfDecay.type = FBParamType::Identity;
  auto selectDSFDecay = [](auto& module) { return &module.acc.dsfDecay; };
  dsfDecay.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFDecay);
  dsfDecay.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectDSFDecay);
  dsfDecay.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFDecay);
  dsfDecay.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF; });

  auto& fmExp = result->params[(int)FFOsciParam::FMExp];
  fmExp.acc = false;
  fmExp.defaultText = "Off";
  fmExp.name = "Exp";
  fmExp.tooltip = "Exponential";
  fmExp.slotCount = 1;
  fmExp.id = "{BE60503A-3CE3-422D-8795-C2FCB1C4A3B6}";
  fmExp.type = FBParamType::Boolean;
  auto selectFMExp = [](auto& module) { return &module.block.fmExp; };
  fmExp.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMExp);
  fmExp.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectFMExp);
  fmExp.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMExp);
  fmExp.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

  auto& fmRatioMode = result->params[(int)FFOsciParam::FMRatioMode];
  fmRatioMode.acc = false;
  fmRatioMode.defaultText = "Ratio";
  fmRatioMode.name = "Ratio";
  fmRatioMode.tooltip = "FM Ratio";
  fmRatioMode.slotCount = 1;
  fmRatioMode.id = "{40838341-6882-4BF8-813F-BA5B89B3042F}";
  fmRatioMode.type = FBParamType::List;
  fmRatioMode.List().items = {
    { "{A6E83AA7-9DA3-4ECA-90DE-BCA123B48203}", "Ratio" },
    { "{B4AF1436-6ED3-4B43-A6F0-F5B624E4121D}", "Free" } };
  auto selectFMRatioMode = [](auto& module) { return &module.block.fmRatioMode; };
  fmRatioMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMRatioMode);
  fmRatioMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectFMRatioMode);
  fmRatioMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMRatioMode);
  fmRatioMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

  auto& fmRatioFree = result->params[(int)FFOsciParam::FMRatioFree];
  fmRatioFree.acc = true;
  fmRatioFree.defaultText = "1";
  fmRatioFree.name = "Ratio";
  fmRatioFree.tooltip = "FM Ratio";
  fmRatioFree.slotCount = FFOsciFMOperatorCount - 1;
  fmRatioFree.id = "{0188A986-8FA9-4BA2-BF84-A1A463712A40}";
  fmRatioFree.type = FBParamType::Log2;
  fmRatioFree.Log2().Init(0.0f, 1.0f / FFOsciFMRatioCount, FFOsciFMRatioCount);
  fmRatioFree.slotFormatter = FFOsciFMFormatRatioSlot;
  auto selectFMRatioFree = [](auto& module) { return &module.acc.fmRatioFree; };
  fmRatioFree.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::FMRatioMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM && vs[1] == (int)FFOsciFMRatioMode::Free; });

  auto& fmRatioRatio = result->params[(int)FFOsciParam::FMRatioRatio];
  fmRatioRatio.acc = false;
  fmRatioRatio.defaultText = "1:1";
  fmRatioRatio.name = "Ratio";
  fmRatioRatio.tooltip = "FM Ratio";
  fmRatioRatio.slotCount = FFOsciFMOperatorCount - 1;
  fmRatioRatio.id = "{9F79D937-E295-41FC-ACFE-F085E12DFF90}";
  fmRatioRatio.type = FBParamType::Discrete;
  fmRatioRatio.slotFormatter = FFOsciFMFormatRatioSlot;
  fmRatioRatio.Discrete().subMenuItemCount = FFOsciFMRatioCount;
  fmRatioRatio.Discrete().valueCount = FFOsciFMRatioCount * FFOsciFMRatioCount;
  fmRatioRatio.Discrete().valueFormatter = FFOsciFMFormatRatioValue;
  fmRatioRatio.Discrete().subMenuFormatter = FFOsciFMFormatRatioSubMenu;
  auto selectFMRatioRatio = [](auto& module) { return &module.block.fmRatioRatio; };
  fmRatioRatio.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMRatioRatio);
  fmRatioRatio.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectFMRatioRatio);
  fmRatioRatio.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMRatioRatio);
  fmRatioRatio.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::FMRatioMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM && vs[1] == (int)FFOsciFMRatioMode::Ratio; });

  auto& fmIndex = result->params[(int)FFOsciParam::FMIndex];
  fmIndex.acc = true;
  fmIndex.defaultText = "0";
  fmIndex.name = "Index";
  fmIndex.tooltip = "FM Index";
  fmIndex.slotCount = FFOsciFMMatrixSize;
  fmIndex.id = "{5CEFAD50-CB71-4E79-B3D6-50B004AD7F03}";
  fmIndex.type = FBParamType::Log2;
  fmIndex.Log2().Init(-0.01, 0.01f, 16.01f);
  fmIndex.slotFormatter = FFOsciFMFormatIndexSlot;
  auto selectFMIndex = [](auto& module) { return &module.acc.fmIndex; };
  fmIndex.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMIndex);
  fmIndex.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

  return result;
}