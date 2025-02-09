#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeOsciTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc";
  result->slotCount = FFOsciCount;
  result->id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  result->params.resize((int)FFOsciParam::Count);
  result->addrSelectors.voiceModuleExchange = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.osci; });
  auto selectModule = [](auto& state) { return &state.voice.osci; };

  auto& on = result->params[(int)FFOsciParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{35FC56D5-F0CB-4C37-BCA2-A0323FA94DCF}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& type = result->params[(int)FFOsciParam::Type];
  type.acc = false;
  type.defaultText = "Sine";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{43F55F08-7C81-44B8-9A95-CC897785D3DE}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{2400822D-BFA9-4A43-91E8-2849756DE659}", "Sine" },
    { "{ECE0331E-DD96-446E-9CCA-5B89EE949EB4}", "Saw" },
    { "{E552CDF9-62A8-4EE2-84E2-8D7170D15919}", "Pulse" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectType);
  type.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectType);
  type.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectType);

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

  auto& gain = result->params[(int)FFOsciParam::Gain];
  gain.acc = true;
  gain.defaultText = "100";
  gain.name = "Gain";
  gain.slotCount = FFOsciGainCount;
  gain.unit = "%";
  gain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  gain.type = FBParamType::Linear;
  gain.Linear().displayMultiplier = 100.0f;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectGain);
  gain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectGain);

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

  auto& pw = result->params[(int)FFOsciParam::PW];
  pw.acc = true;
  pw.defaultText = "50";
  pw.name = "PW";
  pw.slotCount = 1;
  pw.unit = "%";
  pw.id = "{CDB18D21-6C2A-4352-93E1-FCF37EA7D35F}";
  pw.type = FBParamType::Linear;
  pw.Linear().displayMultiplier = 100.0f;
  auto selectPW = [](auto& module) { return &module.acc.pw; };
  pw.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectPW);
  pw.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectPW);
  pw.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectPW);
  pw.relevant.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Pulse; });

  auto& gLFOToGain = result->params[(int)FFOsciParam::GLFOToGain];
  gLFOToGain.acc = true;
  gLFOToGain.name = "GLFO To Gain";
  gLFOToGain.slotCount = 1;
  gLFOToGain.unit = "%";
  gLFOToGain.id = "{5F4BE3D9-EA5F-49D9-B6C5-8FCD0C279B93}";
  gLFOToGain.type = FBParamType::Linear;
  gLFOToGain.Linear().displayMultiplier = 100.0f;
  auto selectGLFOToGain = [](auto& module) { return &module.acc.gLFOToGain; };
  gLFOToGain.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectGLFOToGain);
  gLFOToGain.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectGLFOToGain);
  gLFOToGain.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectGLFOToGain);
  return result;
}