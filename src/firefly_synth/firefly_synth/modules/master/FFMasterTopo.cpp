#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeMasterTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Master";
  result->matrixName = "Mst";
  result->slotCount = 1;
  result->id = "{83AA98D4-9D12-4D61-81A4-4FAA935EDF5D}";
  result->params.resize((int)FFMasterParam::Count);
  result->cvOutputs.resize((int)FFMasterCVOutput::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.master; });
  auto selectModule = [](auto& state) { return &state.global.master; };

  auto& tuningMode = result->params[(int)FFMasterParam::TuningMode];
  tuningMode.mode = FBParamMode::Block;
  tuningMode.name = "Tuning";
  tuningMode.slotCount = 1;
  tuningMode.id = "{90D901C3-CAFE-492A-89F8-FD081D366B75}";
  tuningMode.defaultText = "Off";
  tuningMode.type = FBParamType::List;
  tuningMode.List().items = {
    { "{056E5528-280F-4D99-A1D3-41F459609558}", "Off" },
    { "{34899A66-AB72-4421-9CFB-E692C9F8ACAE}", "Note Once" },
    { "{FB1A7294-D527-4492-B4E9-E0D31EA74409}", "Note Slide" },
    { "{6FE4A9F0-F12E-49FE-BB23-78BD839A6E99}", "Mod Once" },
    { "{138A84B2-29D3-487C-A8EE-A1A1EE0B0C2C}", "Mod Slide" } };
  auto selectTuningMode = [](auto& module) { return &module.block.tuningMode; };
  tuningMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuningMode);
  tuningMode.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuningMode);
  tuningMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuningMode);
  // TODO once we actually do tuning
  tuningMode.dependencies.enabled.audio.WhenSimple({ (int)FFMasterParam::TuningMode }, [](auto const&) { return false; });

  auto& hostSmoothTime = result->params[(int)FFMasterParam::HostSmoothTime];
  hostSmoothTime.mode = FBParamMode::Block;
  hostSmoothTime.defaultText = "2";
  hostSmoothTime.name = "External MIDI/Automation Smoothing";
  hostSmoothTime.display = "Ext. Smth";
  hostSmoothTime.slotCount = 1;
  hostSmoothTime.unit = "Ms";
  hostSmoothTime.id = "{47B38412-40B9-474E-9305-062E7FF7C800}";
  hostSmoothTime.type = FBParamType::Linear;
  hostSmoothTime.Linear().min = 0.0f;
  hostSmoothTime.Linear().max = 0.2f;
  hostSmoothTime.Linear().editSkewFactor = 0.5f;
  hostSmoothTime.Linear().displayMultiplier = 1000.0f;
  auto selectHostSmoothTime = [](auto& module) { return &module.block.hostSmoothTime; };
  hostSmoothTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectHostSmoothTime);;

  auto& aux = result->params[(int)FFMasterParam::Aux];
  aux.mode = FBParamMode::Accurate;
  aux.defaultText = "100";
  aux.name = "Aux";
  aux.slotCount = FFMasterAuxCount;
  aux.unit = "%";
  aux.id = "{BF8F4931-48E3-4277-98ED-950CD76595A9}";
  aux.type = FBParamType::Identity;
  auto selectAux = [](auto& module) { return &module.acc.aux; };
  aux.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAux);
  aux.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectAux);
  aux.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAux);

  auto& modWheel = result->params[(int)FFMasterParam::ModWheel];
  modWheel.mode = FBParamMode::Accurate;
  modWheel.defaultText = "0";
  modWheel.name = "Mod Wheel";
  modWheel.slotCount = 1;
  modWheel.unit = "%";
  modWheel.id = "{74F8A030-AD5A-4CF9-96D7-82123D2DCF24}";
  modWheel.type = FBParamType::Identity;
  auto selectModWheel = [](auto& module) { return &module.acc.modWheel; };
  modWheel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectModWheel);
  modWheel.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectModWheel);
  modWheel.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectModWheel);

  auto& pitchBend = result->params[(int)FFMasterParam::PitchBend];
  pitchBend.mode = FBParamMode::Accurate;
  pitchBend.defaultText = "0";
  pitchBend.name = "Pitch Bend";
  pitchBend.slotCount = 1;
  pitchBend.unit = "%";
  pitchBend.id = "{381145EE-9595-460F-937F-9078F40DBD65}";
  pitchBend.type = FBParamType::Linear;
  pitchBend.Linear().min = -1.0f;
  pitchBend.Linear().max = 1.0f;
  pitchBend.Linear().displayMultiplier = 100;
  auto selectPitchBend = [](auto& module) { return &module.acc.pitchBend; };
  pitchBend.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPitchBend);
  pitchBend.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectPitchBend);
  pitchBend.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPitchBend);

  auto& bendRange = result->params[(int)FFMasterParam::PitchBendRange];
  bendRange.mode = FBParamMode::Block;
  bendRange.name = "Bend Range";
  bendRange.display = "Range";
  bendRange.slotCount = 1;
  bendRange.id = "{71DCB9FB-D46F-413C-9AD5-28F0DD5DA7A5}";
  bendRange.unit = "Semitones";
  bendRange.defaultText = "+/-12";
  bendRange.type = FBParamType::Discrete;
  bendRange.Discrete().valueCount = 24;
  bendRange.Discrete().valueOffset = 1;
  bendRange.Discrete().valueFormatter = [](int v) { 
    return "+/-" + std::to_string(v); };
  auto selectBendRange = [](auto& module) { return &module.block.pitchBendRange; };
  bendRange.scalarAddr = FFSelectScalarParamAddr(selectModule, selectBendRange);
  bendRange.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectBendRange);
  bendRange.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectBendRange);

  auto& bendTarget = result->params[(int)FFMasterParam::PitchBendTarget];
  bendTarget.mode = FBParamMode::Block;
  bendTarget.name = "Bend Target";
  bendTarget.display = "Target";
  bendTarget.slotCount = 1;
  bendTarget.id = "{CA922D71-6FDF-42F7-893D-2286EDAA91F1}";
  bendTarget.defaultText = "Voice";
  bendTarget.type = FBParamType::List;
  bendTarget.List().items = {
    { "{70648B58-31C4-4124-92B4-E16930EFF7CD}", "Off" },
    { "{3298B04A-315C-4E13-95DC-8ABC863E2227}", "Voice" },
    { "{73D34B8C-5227-4819-B246-26EECB524FC1}", "Osc 1" },
    { "{9D1F8C23-9685-49CD-815E-687CBDF22426}", "Osc 2" },
    { "{0E3973C6-A037-4456-B476-74ADDE0A4AAA}", "Osc 3" },
    { "{AE6E127F-6398-440B-883E-5F8025440726}", "Osc 4" } };
  auto selectBendTarget = [](auto& module) { return &module.block.pitchBendTarget; };
  bendTarget.scalarAddr = FFSelectScalarParamAddr(selectModule, selectBendTarget);
  bendTarget.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectBendTarget);
  bendTarget.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectBendTarget);

  auto& uniCount = result->params[(int)FFMasterParam::UniCount];
  uniCount.mode = FBParamMode::Block;
  uniCount.defaultText = "1";
  uniCount.display = "Uni";
  uniCount.name = "Uni Count";
  uniCount.slotCount = 1;
  uniCount.id = "{617F995E-38CC-40BE-899E-AEAE37852092}";
  uniCount.type = FBParamType::Discrete;
  uniCount.Discrete().valueCount = FFMasterUniMaxCount;
  uniCount.Discrete().valueOffset = 1;
  auto selectUniCount = [](auto& module) { return &module.block.uniCount; };
  uniCount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniCount);
  uniCount.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniCount);
  uniCount.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniCount);

  auto& uniOscGain = result->params[(int)FFMasterParam::UniOscGain];
  uniOscGain.mode = FBParamMode::Accurate;
  uniOscGain.defaultText = "100";
  uniOscGain.name = "Osc Gain";
  uniOscGain.slotCount = FFMasterUniMaxCount;
  uniOscGain.unit = "%";
  uniOscGain.id = "{847E5FD5-8DAC-43B1-BA0E-125FD916844C}";
  uniOscGain.type = FBParamType::Identity;
  auto selectUniOscGain = [](auto& module) { return &module.acc.uniOscGain; };
  uniOscGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscGain);
  uniOscGain.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscGain);
  uniOscGain.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscGain);
  uniOscGain.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniOscGain, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniOscPan = result->params[(int)FFMasterParam::UniOscPan];
  uniOscPan.mode = FBParamMode::Accurate;
  uniOscPan.defaultText = "50";
  uniOscPan.name = "Osc Pan";
  uniOscPan.slotCount = FFMasterUniMaxCount;
  uniOscPan.unit = "%";
  uniOscPan.id = "{9CACBCF5-9D85-431B-986B-E2B50DC3336A}";
  uniOscPan.type = FBParamType::Identity;
  auto selectUniOscPan = [](auto& module) { return &module.acc.uniOscPan; };
  uniOscPan.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscPan);
  uniOscPan.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscPan);
  uniOscPan.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscPan);
  uniOscPan.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniOscPan, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniOscCoarse = result->params[(int)FFMasterParam::UniOscCoarse];
  uniOscCoarse.mode = FBParamMode::Accurate;
  uniOscCoarse.defaultText = "0";
  uniOscCoarse.name = "Osc Coarse";
  uniOscCoarse.slotCount = FFMasterUniMaxCount;
  uniOscCoarse.unit = "Semitones";
  uniOscCoarse.id = "{C16C50D5-00C0-4393-87F9-1A7C457F4483}";
  uniOscCoarse.type = FBParamType::Linear;
  uniOscCoarse.Linear().min = -12.0f;
  uniOscCoarse.Linear().max = 12.0f;
  auto selectUniOscCoarse = [](auto& module) { return &module.acc.uniOscCoarse; };
  uniOscCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscCoarse);
  uniOscCoarse.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscCoarse);
  uniOscCoarse.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscCoarse);
  uniOscCoarse.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniOscCoarse, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniOscFine = result->params[(int)FFMasterParam::UniOscFine];
  uniOscFine.mode = FBParamMode::Accurate;
  uniOscFine.defaultText = "0";
  uniOscFine.name = "Osc Fine";
  uniOscFine.slotCount = FFMasterUniMaxCount;
  uniOscFine.unit = "Cent";
  uniOscFine.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  uniOscFine.type = FBParamType::Linear;
  uniOscFine.Linear().min = -1.0f;
  uniOscFine.Linear().max = 1.0f;
  uniOscFine.Linear().displayMultiplier = 100.0f;
  auto selectUniOscFine = [](auto& module) { return &module.acc.uniOscFine; };
  uniOscFine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscFine);
  uniOscFine.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscFine);
  uniOscFine.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscFine);
  uniOscFine.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniOscFine, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });
  
  auto& uniOscPhaseOffset = result->params[(int)FFMasterParam::UniOscPhaseOffset];
  uniOscPhaseOffset.mode = FBParamMode::Block;
  uniOscPhaseOffset.defaultText = "50";
  uniOscPhaseOffset.name = "Uni Osc Phase";
  uniOscPhaseOffset.slotCount = FFMasterUniMaxCount;
  uniOscPhaseOffset.unit = "%";
  uniOscPhaseOffset.id = "{EC98E6C3-0BB3-4E19-A861-5CDDA8C0401F}";
  uniOscPhaseOffset.type = FBParamType::Identity;
  auto selectUniOscPhaseOffset = [](auto& module) { return &module.block.uniOscPhaseOffset; };
  uniOscPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscPhaseOffset);
  uniOscPhaseOffset.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscPhaseOffset);
  uniOscPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscPhaseOffset);
  uniOscPhaseOffset.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniOscPhaseOffset, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniOscPhaseRand = result->params[(int)FFMasterParam::UniOscPhaseRand];
  uniOscPhaseRand.mode = FBParamMode::Block;
  uniOscPhaseRand.defaultText = "50";
  uniOscPhaseRand.name = "Uni Osc Phase Rnd";
  uniOscPhaseRand.slotCount = FFMasterUniMaxCount;
  uniOscPhaseRand.unit = "%";
  uniOscPhaseRand.id = "{8A9F0F51-B8CF-4BBE-A35A-3A5A8A1FA0EF}";
  uniOscPhaseRand.type = FBParamType::Identity;
  auto selectUniOscPhaseRand = [](auto& module) { return &module.block.uniOscPhaseRand; };
  uniOscPhaseRand.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniOscPhaseRand);
  uniOscPhaseRand.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniOscPhaseRand);
  uniOscPhaseRand.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniOscPhaseRand);
  uniOscPhaseRand.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniOscPhaseRand, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& outputAux = result->cvOutputs[(int)FFMasterCVOutput::Aux];
  outputAux.name = "Aux";
  outputAux.slotCount = FFMasterAuxCount;
  outputAux.id = "{928A45BD-5BBF-42EE-97A2-1AD24B69C636}";
  outputAux.globalAddr = [](int, int os, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.master.outputAux[os]; };

  auto& outputMod = result->cvOutputs[(int)FFMasterCVOutput::ModWheel];
  outputMod.name = "Mod";
  outputMod.slotCount = 1;
  outputMod.id = "{385776EE-4199-4100-8EEB-CDC0F6E9C6FF}";
  outputMod.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.master.outputMod; };

  auto& outputPitchBend = result->cvOutputs[(int)FFMasterCVOutput::PitchBend];
  outputPitchBend.name = "PB";
  outputPitchBend.slotCount = 1;
  outputPitchBend.id = "{73FB762F-0C59-4DD3-86D2-D61076C81779}";
  outputPitchBend.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.master.outputBend; };

  return result;
}