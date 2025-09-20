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
  uniOscGain.defaultText = "50";
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
  uniOscPhaseOffset.defaultText = "0";
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
  uniOscPhaseRand.defaultText = "0";
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

  auto& uniLFORate = result->params[(int)FFMasterParam::UniLFORate];
  uniLFORate.mode = FBParamMode::Accurate;
  uniLFORate.defaultText = "50";
  uniLFORate.name = "LFO Rate";
  uniLFORate.slotCount = FFMasterUniMaxCount;
  uniLFORate.unit = "%";
  uniLFORate.id = "{A14D92A7-0952-4DF9-98D6-194B1A763DDF}";
  uniLFORate.type = FBParamType::Identity;
  auto selectUniLFORate = [](auto& module) { return &module.acc.uniLFORate; };
  uniLFORate.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniLFORate);
  uniLFORate.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniLFORate);
  uniLFORate.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniLFORate);
  uniLFORate.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniLFORate, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniLFOMin = result->params[(int)FFMasterParam::UniLFOMin];
  uniLFOMin.mode = FBParamMode::Accurate;
  uniLFOMin.defaultText = "50";
  uniLFOMin.name = "LFO Min";
  uniLFOMin.slotCount = FFMasterUniMaxCount;
  uniLFOMin.unit = "%";
  uniLFOMin.id = "{BC67CD56-29BF-491D-BA5F-43541B6E506D}";
  uniLFOMin.type = FBParamType::Identity;
  auto selectUniLFOMin = [](auto& module) { return &module.acc.uniLFOMin; };
  uniLFOMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniLFOMin);
  uniLFOMin.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniLFOMin);
  uniLFOMin.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniLFOMin);
  uniLFOMin.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniLFOMin, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });
  
  auto& uniLFOMax = result->params[(int)FFMasterParam::UniLFOMax];
  uniLFOMax.mode = FBParamMode::Accurate;
  uniLFOMax.defaultText = "50";
  uniLFOMax.name = "LFO Max";
  uniLFOMax.slotCount = FFMasterUniMaxCount;
  uniLFOMax.unit = "%";
  uniLFOMax.id = "{D6A4D1B5-8961-4C5B-96CA-792F73227A0C}";
  uniLFOMax.type = FBParamType::Identity;
  auto selectUniLFOMax = [](auto& module) { return &module.acc.uniLFOMax; };
  uniLFOMax.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniLFOMax);
  uniLFOMax.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniLFOMax);
  uniLFOMax.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniLFOMax);
  uniLFOMax.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniLFOMax, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniLFOSkewAX = result->params[(int)FFMasterParam::UniLFOSkewAX];
  uniLFOSkewAX.mode = FBParamMode::Accurate;
  uniLFOSkewAX.defaultText = "50";
  uniLFOSkewAX.name = "LFO Skew A X";
  uniLFOSkewAX.slotCount = FFMasterUniMaxCount;
  uniLFOSkewAX.unit = "%";
  uniLFOSkewAX.id = "{5FABD18F-6F53-4532-AC77-7FC5882B2CD1}";
  uniLFOSkewAX.type = FBParamType::Identity;
  auto selectUniLFOSkewAX = [](auto& module) { return &module.acc.uniLFOSkewAX; };
  uniLFOSkewAX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniLFOSkewAX);
  uniLFOSkewAX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniLFOSkewAX);
  uniLFOSkewAX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniLFOSkewAX);
  uniLFOSkewAX.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniLFOSkewAX, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniLFOSkewAY = result->params[(int)FFMasterParam::UniLFOSkewAY];
  uniLFOSkewAY.mode = FBParamMode::Accurate;
  uniLFOSkewAY.defaultText = "50";
  uniLFOSkewAY.name = "LFO Skew A Y";
  uniLFOSkewAY.slotCount = FFMasterUniMaxCount;
  uniLFOSkewAY.unit = "%";
  uniLFOSkewAY.id = "{7C900D23-EC5F-483B-82A3-EB27E2DD9E66}";
  uniLFOSkewAY.type = FBParamType::Identity;
  auto selectUniLFOSkewAY = [](auto& module) { return &module.acc.uniLFOSkewAY; };
  uniLFOSkewAY.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniLFOSkewAY);
  uniLFOSkewAY.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniLFOSkewAY);
  uniLFOSkewAY.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniLFOSkewAY);
  uniLFOSkewAY.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniLFOSkewAY, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniLFOPhaseOffset = result->params[(int)FFMasterParam::UniLFOPhaseOffset];
  uniLFOPhaseOffset.mode = FBParamMode::Block;
  uniLFOPhaseOffset.defaultText = "0";
  uniLFOPhaseOffset.name = "Uni LFO Phase";
  uniLFOPhaseOffset.slotCount = FFMasterUniMaxCount;
  uniLFOPhaseOffset.unit = "%";
  uniLFOPhaseOffset.id = "{00A68371-5419-4398-8B42-4AADF0955DAF}";
  uniLFOPhaseOffset.type = FBParamType::Identity;
  auto selectUniLFOPhaseOffset = [](auto& module) { return &module.block.uniLFOPhaseOffset; };
  uniLFOPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniLFOPhaseOffset);
  uniLFOPhaseOffset.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniLFOPhaseOffset);
  uniLFOPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniLFOPhaseOffset);
  uniLFOPhaseOffset.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniLFOPhaseOffset, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniLFOPhaseRand = result->params[(int)FFMasterParam::UniLFOPhaseRand];
  uniLFOPhaseRand.mode = FBParamMode::Block;
  uniLFOPhaseRand.defaultText = "0";
  uniLFOPhaseRand.name = "Uni LFO Phase Rnd";
  uniLFOPhaseRand.slotCount = FFMasterUniMaxCount;
  uniLFOPhaseRand.unit = "%";
  uniLFOPhaseRand.id = "{78DD7308-DA56-4A61-8940-6FE8F0632EAD}";
  uniLFOPhaseRand.type = FBParamType::Identity;
  auto selectUniLFOPhaseRand = [](auto& module) { return &module.block.uniLFOPhaseRand; };
  uniLFOPhaseRand.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniLFOPhaseRand);
  uniLFOPhaseRand.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniLFOPhaseRand);
  uniLFOPhaseRand.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniLFOPhaseRand);
  uniLFOPhaseRand.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniLFOPhaseRand, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniVFXParamA = result->params[(int)FFMasterParam::UniVFXParamA];
  uniVFXParamA.mode = FBParamMode::Accurate;
  uniVFXParamA.defaultText = "50";
  uniVFXParamA.name = "VFX Param A";
  uniVFXParamA.slotCount = FFMasterUniMaxCount;
  uniVFXParamA.unit = "%";
  uniVFXParamA.id = "{E383FFF3-6549-44A4-878B-9BE81EB4A69F}";
  uniVFXParamA.type = FBParamType::Identity;
  auto selectUniVFXParamA = [](auto& module) { return &module.acc.uniVFXParamA; };
  uniVFXParamA.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniVFXParamA);
  uniVFXParamA.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniVFXParamA);
  uniVFXParamA.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniVFXParamA);
  uniVFXParamA.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniVFXParamA, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniVFXParamB = result->params[(int)FFMasterParam::UniVFXParamB];
  uniVFXParamB.mode = FBParamMode::Accurate;
  uniVFXParamB.defaultText = "50";
  uniVFXParamB.name = "VFX Param B";
  uniVFXParamB.slotCount = FFMasterUniMaxCount;
  uniVFXParamB.unit = "%";
  uniVFXParamB.id = "{82D2D353-D454-496B-BB0E-F7B23D20D698}";
  uniVFXParamB.type = FBParamType::Identity;
  auto selectUniVFXParamB = [](auto& module) { return &module.acc.uniVFXParamB; };
  uniVFXParamB.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniVFXParamB);
  uniVFXParamB.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniVFXParamB);
  uniVFXParamB.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniVFXParamB);
  uniVFXParamB.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniVFXParamB, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniVFXParamC = result->params[(int)FFMasterParam::UniVFXParamC];
  uniVFXParamC.mode = FBParamMode::Accurate;
  uniVFXParamC.defaultText = "50";
  uniVFXParamC.name = "VFX Param C";
  uniVFXParamC.slotCount = FFMasterUniMaxCount;
  uniVFXParamC.unit = "%";
  uniVFXParamC.id = "{55539EFD-3561-4F1F-BDDC-FF0CEC2AA14B}";
  uniVFXParamC.type = FBParamType::Identity;
  auto selectUniVFXParamC = [](auto& module) { return &module.acc.uniVFXParamC; };
  uniVFXParamC.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniVFXParamC);
  uniVFXParamC.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniVFXParamC);
  uniVFXParamC.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniVFXParamC);
  uniVFXParamC.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniVFXParamC, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniVFXParamD = result->params[(int)FFMasterParam::UniVFXParamD];
  uniVFXParamD.mode = FBParamMode::Accurate;
  uniVFXParamD.defaultText = "50";
  uniVFXParamD.name = "VFX Param D";
  uniVFXParamD.slotCount = FFMasterUniMaxCount;
  uniVFXParamD.unit = "%";
  uniVFXParamD.id = "{9346FF6B-068F-4F36-A4CD-68A652CF1A2C}";
  uniVFXParamD.type = FBParamType::Identity;
  auto selectUniVFXParamD = [](auto& module) { return &module.acc.uniVFXParamD; };
  uniVFXParamD.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniVFXParamD);
  uniVFXParamD.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniVFXParamD);
  uniVFXParamD.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniVFXParamD);
  uniVFXParamD.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniVFXParamD, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

  auto& uniEnvOffset = result->params[(int)FFMasterParam::UniEnvOffset];
  uniEnvOffset.mode = FBParamMode::Block;
  uniEnvOffset.defaultText = "0";
  uniEnvOffset.name = "Env Offset";
  uniEnvOffset.slotCount = FFMasterUniMaxCount;
  uniEnvOffset.unit = "Sec";
  uniEnvOffset.id = "{AEDA998E-B0B7-4A78-8C52-F6B809AC5352}";
  uniEnvOffset.type = FBParamType::Linear;
  uniEnvOffset.Linear().min = 0.0f;
  uniEnvOffset.Linear().max = 0.0f;
  uniEnvOffset.Linear().editSkewFactor = 0.5f;
  auto selectUniEnvOffset = [](auto& module) { return &module.block.uniEnvOffset; };
  uniEnvOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniEnvOffset);
  uniEnvOffset.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniEnvOffset);
  uniEnvOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniEnvOffset);
  uniEnvOffset.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniEnvOffset, -1 } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[0] != 1; });

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