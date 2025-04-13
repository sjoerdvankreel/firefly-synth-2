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
  case 0: return "1:2";
  case 1: return "2:3";
  default: assert(false); return "";
  }
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

  auto& note = result->params[(int)FFOsciParam::Note];
  note.acc = false;
  note.defaultText = FBNoteParam::C4Name;
  note.name = "Note";
  note.slotCount = 1;
  note.id = "{592BFC17-0E32-428F-B4B0-E0DF39514BF0}";
  note.type = FBParamType::Note;
  auto selectNote = [](auto& module) { return &module.block.note; };
  note.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectNote);
  note.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectNote);
  note.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectNote);

  auto& cent = result->params[(int)FFOsciParam::Cent];
  cent.acc = true;
  cent.defaultText = "0";
  cent.name = "Cent";
  cent.slotCount = 1;
  cent.unit = "Cent";
  cent.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  cent.type = FBParamType::Linear;
  cent.Linear().min = -1.0f;
  cent.Linear().max = 1.0f;
  cent.Linear().displayMultiplier = 100.0f;
  auto selectCent = [](auto& module) { return &module.acc.cent; };
  cent.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectCent);
  cent.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectCent);
  cent.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectCent);

  auto& unisonCount = result->params[(int)FFOsciParam::UnisonCount];
  unisonCount.acc = false;
  unisonCount.defaultText = "1";
  unisonCount.name = "Unison";
  unisonCount.tooltip = "Unison Count";
  unisonCount.slotCount = 1;
  unisonCount.id = "{60313673-95FE-4B6D-99A6-B628ACDE6D56}";
  unisonCount.type = FBParamType::Discrete;
  unisonCount.Discrete().valueCount = FFOsciUnisonMaxCount;
  unisonCount.Discrete().valueOffset = 1;
  auto selectUnisonCount = [](auto& module) { return &module.block.unisonCount; };
  unisonCount.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonCount);
  unisonCount.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUnisonCount);
  unisonCount.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonCount);

  auto& unisonOffset = result->params[(int)FFOsciParam::UnisonOffset];
  unisonOffset.acc = false;
  unisonOffset.defaultText = "50";
  unisonOffset.name = "Offset";
  unisonOffset.tooltip = "Unison Offset";
  unisonOffset.slotCount = 1;
  unisonOffset.unit = "%";
  unisonOffset.id = "{6F5754E1-BDF4-4685-98FC-8C613128EE8D}";
  unisonOffset.type = FBParamType::Identity;
  auto selectUnisonOffset = [](auto& module) { return &module.block.unisonOffset; };
  unisonOffset.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonOffset);
  unisonOffset.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUnisonOffset);
  unisonOffset.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonOffset);
  unisonOffset.dependencies.enabled.audio.When({ (int)FFOsciParam::UnisonCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& unisonRandom = result->params[(int)FFOsciParam::UnisonRandom];
  unisonRandom.acc = false;
  unisonRandom.defaultText = "50";
  unisonRandom.name = "Random";
  unisonRandom.tooltip = "Unison Random";
  unisonRandom.slotCount = 1;
  unisonRandom.unit = "%";
  unisonRandom.id = "{6F7F6D55-5740-44AB-8442-267A5730E2DA}";
  unisonRandom.type = FBParamType::Identity;
  auto selectUnisonRandom = [](auto& module) { return &module.block.unisonRandom; };
  unisonRandom.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectUnisonRandom);
  unisonRandom.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectUnisonRandom);
  unisonRandom.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectUnisonRandom);
  unisonRandom.dependencies.enabled.audio.When({ (int)FFOsciParam::UnisonCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& unisonDetune = result->params[(int)FFOsciParam::UnisonDetune];
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
  unisonDetune.dependencies.enabled.audio.When({ (int)FFOsciParam::UnisonCount }, [](auto const& vs) { return vs[0] != 1; }); 

  auto& unisonSpread = result->params[(int)FFOsciParam::UnisonSpread];
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
  unisonSpread.dependencies.enabled.audio.When({ (int)FFOsciParam::UnisonCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& basicSinOn = result->params[(int)FFOsciParam::BasicSinOn];
  basicSinOn.acc = false;
  basicSinOn.defaultText = "On";
  basicSinOn.name = "Sin";
  basicSinOn.tooltip = "Sin On";
  basicSinOn.slotCount = 1;
  basicSinOn.id = "{8D481306-C3DF-42C9-AB9E-C7010E3D2891}";
  basicSinOn.type = FBParamType::Boolean;
  auto selectBasicSinOn = [](auto& module) { return &module.block.basicSinOn; };
  basicSinOn.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSinOn);
  basicSinOn.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectBasicSinOn);
  basicSinOn.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSinOn);
  basicSinOn.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic; });

  auto& basicSawOn = result->params[(int)FFOsciParam::BasicSawOn];
  basicSawOn.acc = false;
  basicSawOn.defaultText = "Off";
  basicSawOn.name = "Saw";
  basicSawOn.tooltip = "Saw On";
  basicSawOn.slotCount = 1;
  basicSawOn.id = "{602A1AE1-A722-4C72-B6BD-D8542347683C}";
  basicSawOn.type = FBParamType::Boolean;
  auto selectBasicSawOn = [](auto& module) { return &module.block.basicSawOn; };
  basicSawOn.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSawOn);
  basicSawOn.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectBasicSawOn);
  basicSawOn.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSawOn);
  basicSawOn.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic; });

  auto& basicTriOn = result->params[(int)FFOsciParam::BasicTriOn];
  basicTriOn.acc = false;
  basicTriOn.defaultText = "Off";
  basicTriOn.name = "Tri";
  basicTriOn.tooltip = "Tri On";
  basicTriOn.slotCount = 1;
  basicTriOn.id = "{1A4157EF-570C-4482-9B64-85C7E20F097C}";
  basicTriOn.type = FBParamType::Boolean;
  auto selectBasicTriOn = [](auto& module) { return &module.block.basicTriOn; };
  basicTriOn.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicTriOn);
  basicTriOn.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectBasicTriOn);
  basicTriOn.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicTriOn);
  basicTriOn.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic; });

  auto& basicSqrOn = result->params[(int)FFOsciParam::BasicSqrOn];
  basicSqrOn.acc = false;
  basicSqrOn.defaultText = "Off";
  basicSqrOn.name = "Sqr";
  basicSqrOn.tooltip = "Sqr On";
  basicSqrOn.slotCount = 1;
  basicSqrOn.id = "{79B04938-3E06-4331-95F1-392DBDE40C39}";
  basicSqrOn.type = FBParamType::Boolean;
  auto selectBasicSqrOn = [](auto& module) { return &module.block.basicSqrOn; };
  basicSqrOn.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSqrOn);
  basicSqrOn.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectBasicSqrOn);
  basicSqrOn.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSqrOn);
  basicSqrOn.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic; });

  auto& basicSinGain = result->params[(int)FFOsciParam::BasicSinGain];
  basicSinGain.acc = true;
  basicSinGain.defaultText = "0";
  basicSinGain.name = "Sin Gain";
  basicSinGain.slotCount = 1;
  basicSinGain.unit = "%";
  basicSinGain.id = "{D8544E3C-B79D-4130-8D1B-A7ACAAE2C2EB}";
  basicSinGain.type = FBParamType::Linear;
  basicSinGain.Linear().min = -1.0f;
  basicSinGain.Linear().max = 1.0f;
  basicSinGain.Linear().displayMultiplier = 100.0f;
  auto selectBasicSinGain = [](auto& module) { return &module.acc.basicSinGain; };
  basicSinGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSinGain);
  basicSinGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicSinGain);
  basicSinGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSinGain);
  basicSinGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicSinOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && vs[1] != 0; });

  auto& basicSawGain = result->params[(int)FFOsciParam::BasicSawGain];
  basicSawGain.acc = true;
  basicSawGain.defaultText = "0";
  basicSawGain.name = "Saw Gain";
  basicSawGain.slotCount = 1;
  basicSawGain.unit = "%";
  basicSawGain.id = "{A3163CEF-B04E-49F8-B351-B115415F6B80}";
  basicSawGain.type = FBParamType::Linear;
  basicSawGain.Linear().min = -1.0f;
  basicSawGain.Linear().max = 1.0f;
  basicSawGain.Linear().displayMultiplier = 100.0f;
  auto selectBasicSawGain = [](auto& module) { return &module.acc.basicSawGain; };
  basicSawGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSawGain);
  basicSawGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicSawGain);
  basicSawGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSawGain);
  basicSawGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicSawOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && vs[1] != 0; });

  auto& basicTriGain = result->params[(int)FFOsciParam::BasicTriGain];
  basicTriGain.acc = true;
  basicTriGain.defaultText = "0";
  basicTriGain.name = "Tri Gain";
  basicTriGain.slotCount = 1;
  basicTriGain.unit = "%";
  basicTriGain.id = "{47CA45DE-21B6-41E9-BFC8-70D44F90AEB7}";
  basicTriGain.type = FBParamType::Linear;
  basicTriGain.Linear().min = -1.0f;
  basicTriGain.Linear().max = 1.0f;
  basicTriGain.Linear().displayMultiplier = 100.0f;
  auto selectBasicTriGain = [](auto& module) { return &module.acc.basicTriGain; };
  basicTriGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicTriGain);
  basicTriGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicTriGain);
  basicTriGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicTriGain);
  basicTriGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicTriOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && vs[1] != 0; });

  auto& basicSqrGain = result->params[(int)FFOsciParam::BasicSqrGain];
  basicSqrGain.acc = true;
  basicSqrGain.defaultText = "0";
  basicSqrGain.name = "Sqr Gain";
  basicSqrGain.slotCount = 1;
  basicSqrGain.unit = "%";
  basicSqrGain.id = "{F27DEECD-8AB2-4639-94E9-5AD06F999D25}";
  basicSqrGain.type = FBParamType::Linear;
  basicSqrGain.Linear().min = -1.0f;
  basicSqrGain.Linear().max = 1.0f;
  basicSqrGain.Linear().displayMultiplier = 100.0f;
  auto selectBasicSqrGain = [](auto& module) { return &module.acc.basicSqrGain; };
  basicSqrGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSqrGain);
  basicSqrGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicSqrGain);
  basicSqrGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSqrGain);
  basicSqrGain.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicSqrOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && vs[1] != 0; });

  auto& basicSqrPW = result->params[(int)FFOsciParam::BasicSqrPW];
  basicSqrPW.acc = true;
  basicSqrPW.defaultText = "100";
  basicSqrPW.name = "PW";
  basicSqrPW.slotCount = 1;
  basicSqrPW.unit = "%";
  basicSqrPW.id = "{D25EFEB3-18E0-498C-AA7C-20E1FB474A0F}";
  basicSqrPW.type = FBParamType::Identity;
  auto selectBasicSqrPW = [](auto& module) { return &module.acc.basicSqrPW; };
  basicSqrPW.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSqrPW);
  basicSqrPW.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicSqrPW);
  basicSqrPW.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSqrPW);
  basicSqrPW.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicSqrOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && vs[1] != 0; });

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
  dsfBandwidth.name = "Bandwidth";
  dsfBandwidth.tooltip = "DSF Bandwidth";
  dsfBandwidth.slotCount = 1;
  dsfBandwidth.unit = "%";
  dsfBandwidth.id = "{D3D24159-2A4F-46FB-8E61-749DB07FCC40}";
  dsfBandwidth.type = FBParamType::Identity;
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
  dsfDecay.defaultText = "100";
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

  auto& fmMode = result->params[(int)FFOsciParam::FMMode];
  fmMode.acc = false;
  fmMode.defaultText = "Lin";
  fmMode.name = "Mode";
  fmMode.tooltip = "FM Mode";
  fmMode.slotCount = 1;
  fmMode.id = "{BE60503A-3CE3-422D-8795-C2FCB1C4A3B6}";
  fmMode.type = FBParamType::List;
  fmMode.List().items = {
    { "{BF843AD0-57AB-4A96-87D0-01218570AFEC}", "Lin" },
    { "{7ED6014F-2C3F-4A32-9FB3-C52FA971C66F}", "Exp" } };
  auto selectFMMode = [](auto& module) { return &module.block.fmMode; };
  fmMode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMMode);
  fmMode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectFMMode);
  fmMode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMMode);
  fmMode.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

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
  fmRatioFree.type = FBParamType::Linear;
  fmRatioFree.Linear().min = 1.0f / FFOsciFMRatioCount;
  fmRatioFree.Linear().max = FFOsciFMRatioCount;
  fmRatioFree.slotFormatter = FFOsciFMFormatRatioSlot;
  auto selectFMRatioFree = [](auto& module) { return &module.acc.fmRatioFree; };
  fmRatioFree.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMRatioFree);
  fmRatioFree.dependencies.visible.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::FMRatioMode }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM && vs[1] == (int)FFOsciFMRatioMode::Free; });

  auto& fmRatioRatio = result->params[(int)FFOsciParam::FMRatioRatio];
  fmRatioRatio.acc = false;
  fmRatioRatio.defaultText = "1";
  fmRatioRatio.name = "Ratio";
  fmRatioRatio.tooltip = "FM Ratio";
  fmRatioRatio.slotCount = FFOsciFMOperatorCount - 1;
  fmRatioRatio.id = "{9F79D937-E295-41FC-ACFE-F085E12DFF90}";
  fmRatioRatio.type = FBParamType::Discrete;
  fmRatioRatio.Discrete().valueCount = FFOsciFMRatioCount * FFOsciFMRatioCount;
  fmRatioRatio.slotFormatter = FFOsciFMFormatRatioSlot;
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
  fmIndex.Log2().Init(-0.01, 0.01f, 64.01f);
  fmIndex.slotFormatter = FFOsciFMFormatIndexSlot;
  auto selectFMIndex = [](auto& module) { return &module.acc.fmIndex; };
  fmIndex.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectFMIndex);
  fmIndex.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectFMIndex);
  fmIndex.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::FM; });

  auto& gLFOToGain = result->params[(int)FFOsciParam::GLFOToGain];
  gLFOToGain.acc = true;
  gLFOToGain.name = "GTG";
  gLFOToGain.slotCount = 1;
  gLFOToGain.unit = "%";
  gLFOToGain.id = "{5F4BE3D9-EA5F-49D9-B6C5-8FCD0C279B93}";
  gLFOToGain.type = FBParamType::Identity;
  auto selectGLFOToGain = [](auto& module) { return &module.acc.gLFOToGain; };
  gLFOToGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectGLFOToGain);
  gLFOToGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectGLFOToGain);
  gLFOToGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectGLFOToGain);

  return result;
}