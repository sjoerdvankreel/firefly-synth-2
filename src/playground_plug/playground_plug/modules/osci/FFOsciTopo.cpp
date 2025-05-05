#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/oscis_graph/FFOscisGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

struct FFBasicModeDetails
{
  bool canPW = {};
  bool canSync = {};
};

static std::vector<FFBasicModeDetails> 
MakeBasicModeDetails()
{
  std::vector<FFBasicModeDetails> result;
  result.resize((int)FFOsciBasicMode::Count);
  result[(int)FFOsciBasicMode::Off] = { false, false };
  result[(int)FFOsciBasicMode::Sin] = { false, false };
  result[(int)FFOsciBasicMode::Cos] = { false, false };
  result[(int)FFOsciBasicMode::Saw] = { false, false };
  result[(int)FFOsciBasicMode::Ramp] = { false, false };
  result[(int)FFOsciBasicMode::Sqr] = { false, false };
  result[(int)FFOsciBasicMode::Tri] = { false, false };
  result[(int)FFOsciBasicMode::Trap] = { false, false };
  result[(int)FFOsciBasicMode::SinSqr] = { false, false };
  result[(int)FFOsciBasicMode::SawSqr] = { false, false };
  result[(int)FFOsciBasicMode::SinSaw] = { false, false };
  result[(int)FFOsciBasicMode::SawM1] = { false, false };
  result[(int)FFOsciBasicMode::SqrM1] = { false, false };
  result[(int)FFOsciBasicMode::BSSin] = { false, false };
  result[(int)FFOsciBasicMode::HWSin] = { false, false };
  result[(int)FFOsciBasicMode::FWSin] = { false, false };
  result[(int)FFOsciBasicMode::AltSin] = { false, false };
  result[(int)FFOsciBasicMode::Parabl] = { false, false };
  result[(int)FFOsciBasicMode::HypTri] = { false, false };
  result[(int)FFOsciBasicMode::PWRect] = { true, false };
  result[(int)FFOsciBasicMode::PWSqr] = { true, false };
  result[(int)FFOsciBasicMode::PWHWSaw] = { true, false };
  result[(int)FFOsciBasicMode::PWTriSaw] = { true, false };
  result[(int)FFOsciBasicMode::PWTriPls] = { true, false };
  result[(int)FFOsciBasicMode::PWTrapTri] = { true, false };
  return result;
}

static std::vector<FFBasicModeDetails> const 
BasicModeDetails = MakeBasicModeDetails();

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
    { "{3F55D6D7-5BDF-4B7F-B1E0-2E59B96EA5C0}", "Basic" },
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

  auto& unisonCount = result->params[(int)FFOsciParam::UniCount];
  unisonCount.acc = false;
  unisonCount.defaultText = "1";
  unisonCount.name = "Unison";
  unisonCount.tooltip = "Unison Count";
  unisonCount.slotCount = 1;
  unisonCount.id = "{60313673-95FE-4B6D-99A6-B628ACDE6D56}";
  unisonCount.type = FBParamType::Discrete;
  unisonCount.Discrete().valueCount = FFOsciUniMaxCount;
  unisonCount.Discrete().valueOffset = 1;
  auto selectUnisonCount = [](auto& module) { return &module.block.uniCount; };
  unisonCount.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonCount);
  unisonCount.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUnisonCount);
  unisonCount.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonCount);

  auto& unisonOffset = result->params[(int)FFOsciParam::UniOffset];
  unisonOffset.acc = false;
  unisonOffset.defaultText = "50";
  unisonOffset.name = "Offset";
  unisonOffset.tooltip = "Unison Offset";
  unisonOffset.slotCount = 1;
  unisonOffset.unit = "%";
  unisonOffset.id = "{6F5754E1-BDF4-4685-98FC-8C613128EE8D}";
  unisonOffset.type = FBParamType::Identity;
  auto selectUnisonOffset = [](auto& module) { return &module.block.uniOffset; };
  unisonOffset.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonOffset);
  unisonOffset.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUnisonOffset);
  unisonOffset.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonOffset);
  unisonOffset.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& unisonRandom = result->params[(int)FFOsciParam::UniRandom];
  unisonRandom.acc = false;
  unisonRandom.defaultText = "50";
  unisonRandom.name = "Random";
  unisonRandom.tooltip = "Unison Random";
  unisonRandom.slotCount = 1;
  unisonRandom.unit = "%";
  unisonRandom.id = "{6F7F6D55-5740-44AB-8442-267A5730E2DA}";
  unisonRandom.type = FBParamType::Identity;
  auto selectUnisonRandom = [](auto& module) { return &module.block.uniRandom; };
  unisonRandom.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonRandom);
  unisonRandom.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUnisonRandom);
  unisonRandom.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonRandom);
  unisonRandom.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& unisonDetune = result->params[(int)FFOsciParam::UniDetune];
  unisonDetune.acc = true;
  unisonDetune.defaultText = "33";
  unisonDetune.name = "Detune";
  unisonDetune.tooltip = "Unison Detune";
  unisonDetune.slotCount = 1;
  unisonDetune.unit = "%";
  unisonDetune.id = "{C73A2DFD-0C6D-47FF-A524-CA14A75DF418}";
  unisonDetune.type = FBParamType::Identity;
  auto selectUnisonDetune = [](auto& module) { return &module.acc.unisonDetune; };
  unisonDetune.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonDetune);
  unisonDetune.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectUnisonDetune);
  unisonDetune.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonDetune);
  unisonDetune.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; }); 

  auto& unisonSpread = result->params[(int)FFOsciParam::UniSpread];
  unisonSpread.acc = true;
  unisonSpread.defaultText = "50";
  unisonSpread.name = "Spread";
  unisonSpread.tooltip = "Unison Spread";
  unisonSpread.slotCount = 1;
  unisonSpread.unit = "%";
  unisonSpread.id = "{9F71BAA5-00A2-408B-8CFC-B70D84A7654E}";
  unisonSpread.type = FBParamType::Identity;
  auto selectUnisonSpread = [](auto& module) { return &module.acc.unisonSpread; };
  unisonSpread.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonSpread);
  unisonSpread.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectUnisonSpread);
  unisonSpread.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonSpread);
  unisonSpread.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& unisonBlend = result->params[(int)FFOsciParam::UniBlend];
  unisonBlend.acc = true;
  unisonBlend.defaultText = "100";
  unisonBlend.name = "Blend";
  unisonBlend.tooltip = "Unison Blend";
  unisonBlend.slotCount = 1;
  unisonBlend.unit = "%";
  unisonBlend.id = "{68974AC4-57ED-41E4-9B0F-6DB29E0B6BBB}";
  unisonBlend.type = FBParamType::Identity;
  auto selectUnisonBlend = [](auto& module) { return &module.acc.unisonBlend; };
  unisonBlend.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonBlend);
  unisonBlend.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectUnisonBlend);
  unisonBlend.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonBlend);
  unisonBlend.dependencies.enabled.audio.When({ (int)FFOsciParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& basicMode = result->params[(int)FFOsciParam::BasicMode];
  basicMode.acc = false;
  basicMode.defaultText = "Off";
  basicMode.name = "Mode";
  basicMode.tooltip = "Basic Mode";
  basicMode.slotCount = FFOsciBasicCount;
  basicMode.id = "{E4159ACA-C4A9-4430-8E4A-44EB5DB8557A}";
  basicMode.type = FBParamType::List;
  basicMode.List().items = {
    { "{00880EBC-8E91-44C6-ADD4-4D2BB9B4E945}", "Off" },
    { "{FE9687FE-2A25-4FD3-8138-D775AC0103C6}", "Sin", "Sine" },
    { "{6A17D1AC-C7EB-46DF-B05B-02F4AB34F402}", "Cos", "Cosine" },
    { "{0D1AA9D3-4CA5-4B67-BFFD-5D5F2C5B7161}", "Saw", "Saw" },
    { "{C858202B-8FD4-4C53-93D4-AB2FF019068C}", "Ramp", "Ramp" },
    { "{CCA18B27-785F-4F33-BCE7-89C27CF9EEC2}", "Sqr", "Sqr" },
    { "{8B420C8F-B2F7-4498-8481-678D3DBD10EF}", "Tri", "Triangle" },
    { "{555A6AC2-2F65-438B-BAE3-7151136EC983}", "Trap", "Trapezoid" },
    { "{B559ECEF-AEF2-448D-A2E8-87C76B42C110}", "SinSqr", "Sin Sqr" },
    { "{67B63472-31C1-4C72-AE37-D377FB342E7C}", "SawSqr", "Saw Sqr" },
    { "{AB878F10-8D9A-4C21-AA6D-E18055EAEB9E}", "SinSaw", "Sin Saw" },
    { "{A3FA17C6-E9F7-4BA6-B791-FF847B3A1B0F}", "SawM1", "Saw No Fundamental" },
    { "{40DF0782-F181-4956-A665-29BAD08E0D6F}", "SqrM1", "Sqr No Fundamental" },
    { "{884C404C-9831-4A36-8669-C0281C334DCF}", "BSSin", "Bipolar Sin^2" },
    { "{D96284EC-DB70-4C30-9F40-CC9789C10211}", "HWSin", "Half Rect Sin" },
    { "{AA6D107C-A4D6-41D2-AA4F-DA05923B2555}", "FWSin", "Full Rect Sin" },
    { "{9497D99D-5458-4D26-9633-F2A26B987681}", "AltSin", "Alternating Sin" },
    { "{EE5AC911-DB72-4D22-ABD0-EBBE72141394}", "Parabl", "Parabola" },
    { "{B3BC94E7-226A-483B-B406-C2879EF62560}", "HypTri", "Hyper Tri" },
    { "{4C9F71AC-ECC2-4D07-8058-2D29FB967BF6}", "PWRect", "PW Rect" },
    { "{DFD55382-FBA0-4080-B179-98385452528B}", "PWSqr", "PW Sqr" },
    { "{7DB51B2E-0C60-438C-B285-82D05855057F}", "PWHWSAW", "PW Half Rect Saw" },
    { "{C69D964F-926E-4100-9558-2D43CCE01853}", "PWTriSaw", "PW Tri To Saw" },
    { "{FAFD0A34-62D0-4A85-B450-BAEA8B5AA35C}", "PWTriPls", "PW Tri To Sqr" },
    { "{465F6A72-2EA2-4EB7-974E-600F5A724CE4}", "PWTrapTri", "PW Trap To Tri" }
  };
  basicMode.List().submenuStart[(int)FFOsciBasicMode::Off] = "Off";
  basicMode.List().submenuStart[(int)FFOsciBasicMode::Sin] = "Basic";
  basicMode.List().submenuStart[(int)FFOsciBasicMode::SawM1] = "Fancy";
  basicMode.List().submenuStart[(int)FFOsciBasicMode::PWRect] = "PW";
  auto selectBasicMode = [](auto& module) { return &module.block.basicMode; };
  basicMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicMode);
  basicMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectBasicMode);
  basicMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicMode);
  basicMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic; });

  auto& basicGain = result->params[(int)FFOsciParam::BasicGain];
  basicGain.acc = true;
  basicGain.defaultText = "100";
  basicGain.name = "Gain";
  basicGain.tooltip = "Basic Gain";
  basicGain.slotCount = FFOsciBasicCount;
  basicGain.unit = "%";
  basicGain.id = "{CB7B0BA4-2182-4EA8-9895-1763A29DD9F0}";
  basicGain.type = FBParamType::Linear;
  basicGain.Linear().min = -1.0f;
  basicGain.Linear().max = 1.0f;
  basicGain.Linear().displayMultiplier = 100.0f;
  auto selectBasicGain = [](auto& module) { return &module.acc.basicGain; };
  basicGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicGain);
  basicGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicGain);
  basicGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicGain);
  basicGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && vs[1] != 0; });

  auto& basicPW = result->params[(int)FFOsciParam::BasicPW];
  basicPW.acc = true;
  basicPW.defaultText = "100";
  basicPW.name = "PW";
  basicPW.tooltip = "Basic PW";
  basicPW.slotCount = FFOsciBasicCount;
  basicPW.unit = "%";
  basicPW.id = "{17BF0368-AC81-45B5-87F3-95958A0C02B6}";
  basicPW.type = FBParamType::Identity;
  auto selectBasicPW = [](auto& module) { return &module.acc.basicPW; };
  basicPW.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicPW);
  basicPW.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicPW);
  basicPW.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicPW);
  basicPW.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicMode }, 
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && BasicModeDetails[vs[1]].canPW; });

  auto& basicSync = result->params[(int)FFOsciParam::BasicSync];
  basicSync.acc = true;
  basicSync.defaultText = "0";
  basicSync.name = "Sync";
  basicSync.tooltip = "Basic Sync";
  basicSync.slotCount = FFOsciBasicCount;
  basicSync.unit = "Semitones";
  basicSync.id = "{E58CA58A-088E-4B84-A0FB-E4EAC3812EC0}";
  basicSync.type = FBParamType::Linear;
  basicSync.Linear().min = 0.0f;
  basicSync.Linear().max = 36.0f;
  auto selectBasicSync = [](auto& module) { return &module.acc.basicSync; };
  basicSync.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSync);
  basicSync.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicSync);
  basicSync.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSync);
  basicSync.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicMode }, 
    [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && BasicModeDetails[vs[1]].canSync; });

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