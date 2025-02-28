#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGraph.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeOsciTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc";
  result->slotCount = FFOsciCount;
  result->graphRenderer = FFOsciRenderGraph;
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
  type.ListRealTime().items = {
    { "{449E467A-2DC0-43B0-8487-57C4492F9FE2}", "Off" },
    { "{3F55D6D7-5BDF-4B7F-B1E0-2E59B96EA5C0}", "Basic" },
    { "{19945EB6-4676-492A-BC38-E586A6D3BF6F}", "DSF" } };
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
  gain.type = FBParamType::Linear;
  gain.LinearRealTime().displayMultiplier = 100.0f;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectGain);
  gain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectGain);

  auto& note = result->params[(int)FFOsciParam::Note];
  note.acc = false;
  note.defaultText = FBNoteParamRealTime::C4Name;
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
  cent.LinearRealTime().min = -1.0f;
  cent.LinearRealTime().max = 1.0f;
  cent.LinearRealTime().displayMultiplier = 100.0f;
  auto selectCent = [](auto& module) { return &module.acc.cent; };
  cent.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectCent);
  cent.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectCent);
  cent.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectCent);

  auto& basicSinOn = result->params[(int)FFOsciParam::BasicSinOn];
  basicSinOn.acc = false;
  basicSinOn.defaultText = "Off";
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
  basicSinGain.LinearRealTime().min = -1.0f;
  basicSinGain.LinearRealTime().max = 1.0f;
  basicSinGain.LinearRealTime().displayMultiplier = 100.0f;
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
  basicSawGain.LinearRealTime().min = -1.0f;
  basicSawGain.LinearRealTime().max = 1.0f;
  basicSawGain.LinearRealTime().displayMultiplier = 100.0f;
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
  basicTriGain.LinearRealTime().min = -1.0f;
  basicTriGain.LinearRealTime().max = 1.0f;
  basicTriGain.LinearRealTime().displayMultiplier = 100.0f;
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
  basicSqrGain.LinearRealTime().min = -1.0f;
  basicSqrGain.LinearRealTime().max = 1.0f;
  basicSqrGain.LinearRealTime().displayMultiplier = 100.0f;
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
  basicSqrPW.type = FBParamType::Linear;
  basicSqrPW.LinearRealTime().displayMultiplier = 100.0f;
  auto selectBasicSqrPW = [](auto& module) { return &module.acc.basicSqrPW; };
  basicSqrPW.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectBasicSqrPW);
  basicSqrPW.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectBasicSqrPW);
  basicSqrPW.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectBasicSqrPW);
  basicSqrPW.dependencies.enabled.audio.When({ (int)FFOsciParam::Type, (int)FFOsciParam::BasicSqrOn }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Basic && vs[1] != 0; });

  // TODO maybe discrete log2 param ? or anything that dont warrant a slider? or maybe slider good, not sure
  auto& dsfOvertones = result->params[(int)FFOsciParam::DSFOvertones];
  dsfOvertones.acc = false;
  dsfOvertones.defaultText = "1";
  dsfOvertones.name = "Overtones";
  dsfOvertones.tooltip = "DSF Overtones";
  dsfOvertones.slotCount = 1;
  dsfOvertones.id = "{508C3B8D-E382-4438-B493-81208422E733}";
  dsfOvertones.type = FBParamType::DiscreteLog2;
  dsfOvertones.DiscreteLog2RealTime().valueCount = 11;
  auto selectDSFOvertones = [](auto& module) { return &module.block.dsfOvertones; };
  dsfOvertones.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFOvertones);
  dsfOvertones.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDSFOvertones);
  dsfOvertones.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFOvertones);
  dsfOvertones.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF; });

  auto& dsfDistance = result->params[(int)FFOsciParam::DSFDistance];
  dsfDistance.acc = false;
  dsfDistance.defaultText = "1";
  dsfDistance.name = "Distance";
  dsfDistance.tooltip = "DSF Distance";
  dsfDistance.slotCount = 1;
  dsfDistance.id = "{0D1D4920-A17F-4716-A42E-238DD1E99952}";
  dsfDistance.type = FBParamType::Linear;
  dsfDistance.LinearRealTime().min = 0.05f;
  dsfDistance.LinearRealTime().max = 20.0f;
  auto selectDSFDistance = [](auto& module) { return &module.block.dsfDistance; };
  dsfDistance.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFDistance);
  dsfDistance.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDSFDistance);
  dsfDistance.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFDistance);
  dsfDistance.dependencies.enabled.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF; }); 
  
  auto& dsfDecay = result->params[(int)FFOsciParam::DSFDecay];
  dsfDecay.acc = true;
  dsfDecay.name = "Decay";
  dsfDecay.tooltip = "DSF Decay";
  dsfDecay.slotCount = 1;
  dsfDecay.unit = "%";
  dsfDecay.id = "{21CE915D-0983-4545-9F6E-8743CAC5EAB7}";
  dsfDecay.type = FBParamType::Linear;
  dsfDecay.LinearRealTime().displayMultiplier = 100.0f;
  auto selectDSFDecay = [](auto& module) { return &module.acc.dsfDecay; };
  dsfDecay.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDSFDecay);
  dsfDecay.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectDSFDecay);
  dsfDecay.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDSFDecay);

  auto& gLFOToGain = result->params[(int)FFOsciParam::GLFOToGain];
  gLFOToGain.acc = true;
  gLFOToGain.name = "GLFO To Gain";
  gLFOToGain.slotCount = 1;
  gLFOToGain.unit = "%";
  gLFOToGain.id = "{5F4BE3D9-EA5F-49D9-B6C5-8FCD0C279B93}";
  gLFOToGain.type = FBParamType::Linear;
  gLFOToGain.LinearRealTime().displayMultiplier = 100.0f;
  auto selectGLFOToGain = [](auto& module) { return &module.acc.gLFOToGain; };
  gLFOToGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectGLFOToGain);
  gLFOToGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectGLFOToGain);
  gLFOToGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectGLFOToGain);

  return result;
}