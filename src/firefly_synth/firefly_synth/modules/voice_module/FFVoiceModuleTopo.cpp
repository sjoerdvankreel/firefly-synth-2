#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakePortaBarsItems()
{
  return FBMakeBarsItems(true, { 1, 128 }, { 4, 1 });
}

std::unique_ptr<FBStaticModule>
FFMakeVoiceModuleTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Voice";
  result->matrixName = "Voice";
  result->slotCount = 1;
  result->id = "{AFA1D831-2F91-4FA4-9CBA-958F4AD32DA8}";
  result->params.resize((int)FFVoiceModuleParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.voiceModule; });
  auto selectModule = [](auto& state) { return &state.voice.voiceModule; };

  auto& coarse = result->params[(int)FFVoiceModuleParam::Coarse];
  coarse.mode = FBParamMode::Accurate;
  coarse.defaultText = "0";
  coarse.name = "Coarse";
  coarse.slotCount = 1;
  coarse.unit = "Semitones";
  coarse.id = "{64019FB1-6EE2-4FF5-8D28-F6B692B25EFB}";
  coarse.type = FBParamType::Linear;
  coarse.Linear().min = -128.0f;
  coarse.Linear().max = 128.0f;
  auto selectCoarse = [](auto& module) { return &module.acc.coarse; };
  coarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCoarse);
  coarse.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCoarse);
  coarse.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCoarse);

  auto& env5ToCoarse = result->params[(int)FFVoiceModuleParam::Env5ToCoarse];
  env5ToCoarse.mode = FBParamMode::Accurate;
  env5ToCoarse.name = "Env 5\U00002192Coarse";
  env5ToCoarse.defaultText = "0";
  env5ToCoarse.slotCount = 1;
  env5ToCoarse.unit = "%";
  env5ToCoarse.id = "{7A005131-8806-4815-AEAB-A4B4861B707F}";
  env5ToCoarse.type = FBParamType::Identity;
  auto selectEnv5ToCoarse = [](auto& module) { return &module.acc.env5ToCoarse; };
  env5ToCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectEnv5ToCoarse);
  env5ToCoarse.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectEnv5ToCoarse);
  env5ToCoarse.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectEnv5ToCoarse);

  auto& fine = result->params[(int)FFVoiceModuleParam::Fine];
  fine.mode = FBParamMode::Accurate;
  fine.defaultText = "0";
  fine.name = "Fine";
  fine.slotCount = 1;
  fine.unit = "Cent";
  fine.id = "{52BEBA97-1C64-472A-A424-A03FD8DB5B94}";
  fine.type = FBParamType::Linear;
  fine.Linear().min = -1.0f;
  fine.Linear().max = 1.0f;
  fine.Linear().displayMultiplier = 100.0f;
  auto selectFine = [](auto& module) { return &module.acc.fine; };
  fine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFine);
  fine.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectFine);
  fine.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFine);

  auto& lfo5ToFine = result->params[(int)FFVoiceModuleParam::LFO5ToFine];
  lfo5ToFine.mode = FBParamMode::Accurate;
  lfo5ToFine.name = "VLFO 5\U00002192Fine";
  lfo5ToFine.defaultText = "0";
  lfo5ToFine.slotCount = 1;
  lfo5ToFine.unit = "%";
  lfo5ToFine.id = "{DAF25F23-63B2-4120-98B7-EE28D054CA43}";
  lfo5ToFine.type = FBParamType::Identity;
  auto selectLFO5ToFine = [](auto& module) { return &module.acc.lfo5ToFine; };
  lfo5ToFine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLFO5ToFine);
  lfo5ToFine.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectLFO5ToFine);
  lfo5ToFine.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLFO5ToFine);

  auto& portaType = result->params[(int)FFVoiceModuleParam::PortaType];
  portaType.mode = FBParamMode::Block;
  portaType.name = "Portamento Type";
  portaType.display = "Porta";
  portaType.slotCount = 1;
  portaType.id = "{7F787457-02DE-4D98-ABB1-9E776A2DCBCD}";
  portaType.defaultText = "Off";
  portaType.type = FBParamType::List;
  portaType.List().items = {
    { "{AD9B2ADF-00DF-4411-90A3-B62146458B08}", "Off" },
    { "{9B4ACEB6-CEC6-4D44-8CB3-3CBB94BA3497}", "On" },
    { "{37679449-D1EF-4D04-864E-A577DC24D2EE}", "Auto" } };
  auto selectPortaType = [](auto& module) { return &module.block.portaType; };
  portaType.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPortaType);
  portaType.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectPortaType);
  portaType.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPortaType);

  auto& portaMode = result->params[(int)FFVoiceModuleParam::PortaMode];
  portaMode.mode = FBParamMode::Block;
  portaMode.name = "Portamento Mode";
  portaMode.display = "Mode";
  portaMode.slotCount = 1;
  portaMode.id = "{1A0A6BC7-58A8-4017-B169-C48E25CEF74C}";
  portaMode.defaultText = "Always";
  portaMode.type = FBParamType::List;
  portaMode.List().items = {
    { "{9632987C-7DF1-4B3C-86A7-40FB254D8A97}", "Always" },
    { "{E864BC41-6BC0-491C-9A95-8890A2F94A62}", "Section" } };
  auto selectPortaMode = [](auto& module) { return &module.block.portaMode; };
  portaMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPortaMode);
  portaMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectPortaMode);
  portaMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPortaMode);
  portaMode.dependencies.enabled.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaType }, [](auto const& vs) { return vs[0] != 0; });

  auto& portaSync = result->params[(int)FFVoiceModuleParam::PortaSync];
  portaSync.mode = FBParamMode::Block;
  portaSync.name = "Portamento Tempo Sync";
  portaSync.display = "Sync";
  portaSync.slotCount = 1;
  portaSync.id = "{779E0303-8799-40C6-B654-85FC75FD8DEC}";
  portaSync.defaultText = "Off";
  portaSync.type = FBParamType::Boolean;
  auto selectPortaSync = [](auto& module) { return &module.block.portaSync; };
  portaSync.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPortaSync);
  portaSync.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectPortaSync);
  portaSync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPortaSync);
  portaSync.dependencies.enabled.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaType }, [](auto const& vs) { return vs[0] != 0; });

  auto& portaTime = result->params[(int)FFVoiceModuleParam::PortaTime];
  portaTime.mode = FBParamMode::VoiceStart;
  portaTime.defaultText = "0.1";
  portaTime.display = "Length";
  portaTime.name = "Portamento Length Time";
  portaTime.slotCount = 1;
  portaTime.unit = "Sec";
  portaTime.id = "{C3EF7F24-84C9-4F92-8D18-F4332E0BB874}";
  portaTime.type = FBParamType::Linear;
  portaTime.Linear().min = 0.0f;
  portaTime.Linear().max = 10.0f;
  portaTime.Linear().editSkewFactor = 0.5f;
  auto selectPortaTime = [](auto& module) { return &module.voiceStart.portaTime; };
  portaTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPortaTime);
  portaTime.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectPortaTime);
  portaTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPortaTime);
  portaTime.dependencies.visible.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaSync }, [](auto const& vs) { return vs[0] == 0; });
  portaTime.dependencies.enabled.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaType, (int)FFVoiceModuleParam::PortaSync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& portaBars = result->params[(int)FFVoiceModuleParam::PortaBars];
  portaBars.mode = FBParamMode::Block;
  portaBars.defaultText = "1/16";
  portaBars.display = "Length";
  portaBars.name = "Portamento Length Bars";
  portaBars.slotCount = 1;
  portaBars.unit = "Bars";
  portaBars.id = "{983E1FAD-94A6-413C-A24B-0178D38A84B0}";
  portaBars.type = FBParamType::Bars;
  portaBars.Bars().items = MakePortaBarsItems();
  auto selectPortaBars = [](auto& module) { return &module.block.portaBars; };
  portaBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPortaBars);
  portaBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectPortaBars);
  portaBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPortaBars);
  portaBars.dependencies.visible.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaSync }, [](auto const& vs) { return vs[0] != 0; });
  portaBars.dependencies.enabled.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaType, (int)FFVoiceModuleParam::PortaSync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& portaSectionAmpAttack = result->params[(int)FFVoiceModuleParam::PortaSectionAmpAttack];
  portaSectionAmpAttack.mode = FBParamMode::VoiceStart;
  portaSectionAmpAttack.defaultText = "50";
  portaSectionAmpAttack.name = "Portamento Section Amp Attack";
  portaSectionAmpAttack.display = "Amp Attack";
  portaSectionAmpAttack.slotCount = 1;
  portaSectionAmpAttack.unit = "%";
  portaSectionAmpAttack.id = "{C035EBA5-D5E6-4B3B-A40A-2BD5B500ACCA}";
  portaSectionAmpAttack.type = FBParamType::Identity;
  auto selectPortaSectionAmpAttack = [](auto& module) { return &module.voiceStart.portaSectionAmpAttack; };
  portaSectionAmpAttack.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPortaSectionAmpAttack);
  portaSectionAmpAttack.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectPortaSectionAmpAttack);
  portaSectionAmpAttack.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPortaSectionAmpAttack);
  portaSectionAmpAttack.dependencies.enabled.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaType, (int)FFVoiceModuleParam::PortaMode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFVoiceModulePortaMode::Section; });

  auto& portaSectionAmpRelease = result->params[(int)FFVoiceModuleParam::PortaSectionAmpRelease];
  portaSectionAmpRelease.mode = FBParamMode::VoiceStart;
  portaSectionAmpRelease.defaultText = "50";
  portaSectionAmpRelease.name = "Portamento Section Amp Release";
  portaSectionAmpRelease.display = "Amp Release";
  portaSectionAmpRelease.slotCount = 1;
  portaSectionAmpRelease.unit = "%";
  portaSectionAmpRelease.id = "{227ACA77-CD6E-488B-86BB-9802FDF2D2F2}";
  portaSectionAmpRelease.type = FBParamType::Identity;
  auto selectPortaSectionAmpRelease = [](auto& module) { return &module.voiceStart.portaSectionAmpRelease; };
  portaSectionAmpRelease.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPortaSectionAmpRelease);
  portaSectionAmpRelease.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectPortaSectionAmpRelease);
  portaSectionAmpRelease.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPortaSectionAmpRelease);
  portaSectionAmpRelease.dependencies.enabled.audio.WhenSimple({ (int)FFVoiceModuleParam::PortaType, (int)FFVoiceModuleParam::PortaMode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFVoiceModulePortaMode::Section; });

  auto& uniCount = result->params[(int)FFVoiceModuleParam::UniCount];
  uniCount.mode = FBParamMode::Block;
  uniCount.defaultText = "1";
  uniCount.display = "Uni";
  uniCount.name = "Uni Count";
  uniCount.slotCount = 1;
  uniCount.id = "{617F995E-38CC-40BE-899E-AEAE37852092}";
  uniCount.type = FBParamType::Discrete;
  uniCount.Discrete().valueCount = FFVoiceModuleUniMaxCount;
  uniCount.Discrete().valueOffset = 1;
  auto selectUniCount = [](auto& module) { return &module.block.uniCount; };
  uniCount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniCount);
  uniCount.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniCount);
  uniCount.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniCount);

  auto& uniOscGain = result->params[(int)FFVoiceModuleParam::UniOscGain];
  uniOscGain.mode = FBParamMode::Accurate;
  uniOscGain.defaultText = "100";
  uniOscGain.name = "Osc Gain";
  uniOscGain.slotCount = FFVoiceModuleUniMaxCount;
  uniOscGain.unit = "%";
  uniOscGain.id = "{847E5FD5-8DAC-43B1-BA0E-125FD916844C}";
  uniOscGain.type = FBParamType::Identity;
  auto selectUniOscGain = [](auto& module) { return &module.acc.uniOscGain; };
  uniOscGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscGain);
  uniOscGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscGain);
  uniOscGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscGain);
  uniOscGain.dependencies.enabled.audio.WhenSlots({ { (int)FFVoiceModuleParam::UniCount, -1 }, { (int)FFVoiceModuleParam::UniOscGain, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniOscPan = result->params[(int)FFVoiceModuleParam::UniOscPan];
  uniOscPan.mode = FBParamMode::Accurate;
  uniOscPan.defaultText = "50";
  uniOscPan.name = "Pan";
  uniOscPan.slotCount = FFVoiceModuleUniMaxCount;
  uniOscPan.unit = "%";
  uniOscPan.id = "{9CACBCF5-9D85-431B-986B-E2B50DC3336A}";
  uniOscPan.type = FBParamType::Identity;
  auto selectUniOscPan = [](auto& module) { return &module.acc.uniOscPan; };
  uniOscPan.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscPan);
  uniOscPan.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscPan);
  uniOscPan.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscPan);
  uniOscPan.dependencies.enabled.audio.WhenSlots({ { (int)FFVoiceModuleParam::UniCount, -1 }, { (int)FFVoiceModuleParam::UniOscPan, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniOscCoarse = result->params[(int)FFVoiceModuleParam::UniOscCoarse];
  uniOscCoarse.mode = FBParamMode::Accurate;
  uniOscCoarse.defaultText = "0";
  uniOscCoarse.name = "Osc Coarse";
  uniOscCoarse.slotCount = FFVoiceModuleUniMaxCount;
  uniOscCoarse.unit = "Semitones";
  uniOscCoarse.id = "{C16C50D5-00C0-4393-87F9-1A7C457F4483}";
  uniOscCoarse.type = FBParamType::Linear;
  uniOscCoarse.Linear().min = -12.0f;
  uniOscCoarse.Linear().max = 12.0f;
  auto selectUniOscCoarse = [](auto& module) { return &module.acc.uniOscCoarse; };
  uniOscCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscCoarse);
  uniOscCoarse.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscCoarse);
  uniOscCoarse.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscCoarse);
  coarse.dependencies.enabled.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& fine = result->params[(int)FFOsciParam::Fine];
  fine.mode = FBParamMode::Accurate;
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
  fine.dependencies.enabled.audio.WhenSimple({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}