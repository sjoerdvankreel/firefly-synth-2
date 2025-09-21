#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGlobalUniTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Global Uni";
  result->matrixName = "G.Uni";
  result->slotCount = 1;
  result->id = "{9A5A03DE-E59E-456D-A04C-51F603484C4E}";
  result->params.resize((int)FFGlobalUniParam::Count);
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

  auto& uniType = result->params[(int)FFMasterParam::UniType];
  uniType.mode = FBParamMode::Block;
  uniType.name = "Uni Type";
  uniType.display = "Uni";
  uniType.slotCount = 1;
  uniType.id = "{C5397E08-EB50-4521-9B88-A8D7DFE49208}";
  uniType.defaultText = "Off";
  uniType.type = FBParamType::List;
  uniType.List().items = {
    { "{59453D1D-06AB-4541-ACC6-CE39AC70040C}", "Off" },
    { "{99BA36B5-BFD5-4E9D-BFDF-31860C3B2F4C}", "Basic" },
    { "{D86C0FF4-10B2-416F-B5E3-DBB38A706C70}", "Full" } };
  auto selectUniType = [](auto& module) { return &module.block.uniType; };
  uniType.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniType);
  uniType.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniType);
  uniType.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniType);

  auto& uniCount = result->params[(int)FFMasterParam::UniCount];
  uniCount.mode = FBParamMode::Block;
  uniCount.defaultText = "2";
  uniCount.name = "Uni Count";
  uniCount.slotCount = 1;
  uniCount.id = "{617F995E-38CC-40BE-899E-AEAE37852092}";
  uniCount.type = FBParamType::Discrete;
  uniCount.Discrete().valueCount = FFMasterUniMaxCount - 1;
  uniCount.Discrete().valueOffset = 2;
  auto selectUniCount = [](auto& module) { return &module.block.uniCount; };
  uniCount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniCount);
  uniCount.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniCount);
  uniCount.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniCount);
  uniCount.dependencies.enabled.audio.WhenSimple({ (int)FFMasterParam::UniType }, [](auto const& vs) { return vs[0] != 0; });

  auto& uniFullOscGain = result->params[(int)FFMasterParam::UniFullOscGain];
  uniFullOscGain.mode = FBParamMode::Accurate;
  uniFullOscGain.defaultText = "50";
  uniFullOscGain.name = "Uni Osc Gain";
  uniFullOscGain.display = "Osc Gain";
  uniFullOscGain.slotCount = FFMasterUniMaxCount;
  uniFullOscGain.unit = "%";
  uniFullOscGain.id = "{847E5FD5-8DAC-43B1-BA0E-125FD916844C}";
  uniFullOscGain.type = FBParamType::Identity;
  auto selectUniFullOscGain = [](auto& module) { return &module.acc.uniFullOscGain; };
  uniFullOscGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullOscGain);
  uniFullOscGain.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullOscGain);
  uniFullOscGain.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullOscGain);
  uniFullOscGain.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullOscGain, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullOscPan = result->params[(int)FFMasterParam::UniFullOscPan];
  uniFullOscPan.mode = FBParamMode::Accurate;
  uniFullOscPan.defaultText = "50";
  uniFullOscPan.name = "Uni Osc Pan";
  uniFullOscPan.display = "Osc Pan";
  uniFullOscPan.slotCount = FFMasterUniMaxCount;
  uniFullOscPan.unit = "%";
  uniFullOscPan.id = "{9CACBCF5-9D85-431B-986B-E2B50DC3336A}";
  uniFullOscPan.type = FBParamType::Identity;
  auto selectUniFullOscPan = [](auto& module) { return &module.acc.uniFullOscPan; };
  uniFullOscPan.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullOscPan);
  uniFullOscPan.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullOscPan);
  uniFullOscPan.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullOscPan);
  uniFullOscPan.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullOscPan, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullOscCoarse = result->params[(int)FFMasterParam::UniFullOscCoarse];
  uniFullOscCoarse.mode = FBParamMode::Accurate;
  uniFullOscCoarse.defaultText = "0";
  uniFullOscCoarse.name = "Uni Osc Coarse";
  uniFullOscCoarse.display = "Osc Coarse";
  uniFullOscCoarse.slotCount = FFMasterUniMaxCount;
  uniFullOscCoarse.unit = "Semitones";
  uniFullOscCoarse.id = "{C16C50D5-00C0-4393-87F9-1A7C457F4483}";
  uniFullOscCoarse.type = FBParamType::Linear;
  uniFullOscCoarse.Linear().min = -12.0f;
  uniFullOscCoarse.Linear().max = 12.0f;
  auto selectUniFullOscCoarse = [](auto& module) { return &module.acc.uniFullOscCoarse; };
  uniFullOscCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullOscCoarse);
  uniFullOscCoarse.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullOscCoarse);
  uniFullOscCoarse.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullOscCoarse);
  uniFullOscCoarse.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullOscCoarse, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullOscFine = result->params[(int)FFMasterParam::UniFullOscFine];
  uniFullOscFine.mode = FBParamMode::Accurate;
  uniFullOscFine.defaultText = "0";
  uniFullOscFine.name = "Uni Osc Fine";
  uniFullOscFine.display = "Osc Fine";
  uniFullOscFine.slotCount = FFMasterUniMaxCount;
  uniFullOscFine.unit = "Cent";
  uniFullOscFine.id = "{D32C05E9-BE89-43CB-887F-AF7B1001518F}";
  uniFullOscFine.type = FBParamType::Linear;
  uniFullOscFine.Linear().min = -1.0f;
  uniFullOscFine.Linear().max = 1.0f;
  uniFullOscFine.Linear().displayMultiplier = 100.0f;
  auto selectUniFullOscFine = [](auto& module) { return &module.acc.uniFullOscFine; };
  uniFullOscFine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullOscFine);
  uniFullOscFine.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullOscFine);
  uniFullOscFine.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullOscFine);
  uniFullOscFine.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullOscFine, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });
  
  auto& uniFullOscPhaseOffset = result->params[(int)FFMasterParam::UniFullOscPhaseOffset];
  uniFullOscPhaseOffset.mode = FBParamMode::Block;
  uniFullOscPhaseOffset.defaultText = "0";
  uniFullOscPhaseOffset.name = "Uni Osc Phase";
  uniFullOscPhaseOffset.display = "Osc Phase";
  uniFullOscPhaseOffset.slotCount = FFMasterUniMaxCount;
  uniFullOscPhaseOffset.unit = "%";
  uniFullOscPhaseOffset.id = "{EC98E6C3-0BB3-4E19-A861-5CDDA8C0401F}";
  uniFullOscPhaseOffset.type = FBParamType::Identity;
  auto selectUniFullOscPhaseOffset = [](auto& module) { return &module.block.uniFullOscPhaseOffset; };
  uniFullOscPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullOscPhaseOffset);
  uniFullOscPhaseOffset.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullOscPhaseOffset);
  uniFullOscPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullOscPhaseOffset);
  uniFullOscPhaseOffset.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullOscPhaseOffset, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullOscPhaseRand = result->params[(int)FFMasterParam::UniFullOscPhaseRand];
  uniFullOscPhaseRand.mode = FBParamMode::Block;
  uniFullOscPhaseRand.defaultText = "0";
  uniFullOscPhaseRand.name = "Uni Osc Phs Rnd";
  uniFullOscPhaseRand.display = "Osc Phs Rnd";
  uniFullOscPhaseRand.slotCount = FFMasterUniMaxCount;
  uniFullOscPhaseRand.unit = "%";
  uniFullOscPhaseRand.id = "{8A9F0F51-B8CF-4BBE-A35A-3A5A8A1FA0EF}";
  uniFullOscPhaseRand.type = FBParamType::Identity;
  auto selectUniFullOscPhaseRand = [](auto& module) { return &module.block.uniFullOscPhaseRand; };
  uniFullOscPhaseRand.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullOscPhaseRand);
  uniFullOscPhaseRand.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullOscPhaseRand);
  uniFullOscPhaseRand.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullOscPhaseRand);
  uniFullOscPhaseRand.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullOscPhaseRand, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullLFORate = result->params[(int)FFMasterParam::UniFullLFORate];
  uniFullLFORate.mode = FBParamMode::Accurate;
  uniFullLFORate.defaultText = "50";
  uniFullLFORate.name = "Uni LFO Rate";
  uniFullLFORate.display = "LFO Rate";
  uniFullLFORate.slotCount = FFMasterUniMaxCount;
  uniFullLFORate.unit = "%";
  uniFullLFORate.id = "{A14D92A7-0952-4DF9-98D6-194B1A763DDF}";
  uniFullLFORate.type = FBParamType::Identity;
  auto selectUniFullLFORate = [](auto& module) { return &module.acc.uniFullLFORate; };
  uniFullLFORate.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullLFORate);
  uniFullLFORate.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullLFORate);
  uniFullLFORate.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullLFORate);
  uniFullLFORate.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullLFORate, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullLFOMin = result->params[(int)FFMasterParam::UniFullLFOMin];
  uniFullLFOMin.mode = FBParamMode::Accurate;
  uniFullLFOMin.defaultText = "50";
  uniFullLFOMin.name = "Uni LFO Min";
  uniFullLFOMin.display = "LFO Min";
  uniFullLFOMin.slotCount = FFMasterUniMaxCount;
  uniFullLFOMin.unit = "%";
  uniFullLFOMin.id = "{BC67CD56-29BF-491D-BA5F-43541B6E506D}";
  uniFullLFOMin.type = FBParamType::Identity;
  auto selectUniFullLFOMin = [](auto& module) { return &module.acc.uniFullLFOMin; };
  uniFullLFOMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullLFOMin);
  uniFullLFOMin.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullLFOMin);
  uniFullLFOMin.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullLFOMin);
  uniFullLFOMin.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullLFOMin, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });
  
  auto& uniFullLFOMax = result->params[(int)FFMasterParam::UniFullLFOMax];
  uniFullLFOMax.mode = FBParamMode::Accurate;
  uniFullLFOMax.defaultText = "50";
  uniFullLFOMax.name = "Uni LFO Max";
  uniFullLFOMax.display = "LFO Max";
  uniFullLFOMax.slotCount = FFMasterUniMaxCount;
  uniFullLFOMax.unit = "%";
  uniFullLFOMax.id = "{D6A4D1B5-8961-4C5B-96CA-792F73227A0C}";
  uniFullLFOMax.type = FBParamType::Identity;
  auto selectUniFullLFOMax = [](auto& module) { return &module.acc.uniFullLFOMax; };
  uniFullLFOMax.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullLFOMax);
  uniFullLFOMax.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullLFOMax);
  uniFullLFOMax.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullLFOMax);
  uniFullLFOMax.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullLFOMax, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullLFOSkewAX = result->params[(int)FFMasterParam::UniFullLFOSkewAX];
  uniFullLFOSkewAX.mode = FBParamMode::Accurate;
  uniFullLFOSkewAX.defaultText = "50";
  uniFullLFOSkewAX.name = "Uni LFO SkewA X";
  uniFullLFOSkewAX.display = "LFO SkewA X";
  uniFullLFOSkewAX.slotCount = FFMasterUniMaxCount;
  uniFullLFOSkewAX.unit = "%";
  uniFullLFOSkewAX.id = "{5FABD18F-6F53-4532-AC77-7FC5882B2CD1}";
  uniFullLFOSkewAX.type = FBParamType::Identity;
  auto selectUniFullLFOSkewAX = [](auto& module) { return &module.acc.uniFullLFOSkewAX; };
  uniFullLFOSkewAX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullLFOSkewAX);
  uniFullLFOSkewAX.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullLFOSkewAX);
  uniFullLFOSkewAX.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullLFOSkewAX);
  uniFullLFOSkewAX.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullLFOSkewAX, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullLFOSkewAY = result->params[(int)FFMasterParam::UniFullLFOSkewAY];
  uniFullLFOSkewAY.mode = FBParamMode::Accurate;
  uniFullLFOSkewAY.defaultText = "50";
  uniFullLFOSkewAY.name = "Uni LFO SkewA Y";
  uniFullLFOSkewAY.display = "LFO SkewA Y";
  uniFullLFOSkewAY.slotCount = FFMasterUniMaxCount;
  uniFullLFOSkewAY.unit = "%";
  uniFullLFOSkewAY.id = "{7C900D23-EC5F-483B-82A3-EB27E2DD9E66}";
  uniFullLFOSkewAY.type = FBParamType::Identity;
  auto selectUniFullLFOSkewAY = [](auto& module) { return &module.acc.uniFullLFOSkewAY; };
  uniFullLFOSkewAY.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullLFOSkewAY);
  uniFullLFOSkewAY.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullLFOSkewAY);
  uniFullLFOSkewAY.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullLFOSkewAY);
  uniFullLFOSkewAY.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullLFOSkewAY, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullLFOPhaseOffset = result->params[(int)FFMasterParam::UniFullLFOPhaseOffset];
  uniFullLFOPhaseOffset.mode = FBParamMode::Block;
  uniFullLFOPhaseOffset.defaultText = "0";
  uniFullLFOPhaseOffset.name = "Uni LFO Phase";
  uniFullLFOPhaseOffset.display = "LFO Phase";
  uniFullLFOPhaseOffset.slotCount = FFMasterUniMaxCount;
  uniFullLFOPhaseOffset.unit = "%";
  uniFullLFOPhaseOffset.id = "{00A68371-5419-4398-8B42-4AADF0955DAF}";
  uniFullLFOPhaseOffset.type = FBParamType::Identity;
  auto selectUniFullLFOPhaseOffset = [](auto& module) { return &module.block.uniFullLFOPhaseOffset; };
  uniFullLFOPhaseOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullLFOPhaseOffset);
  uniFullLFOPhaseOffset.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullLFOPhaseOffset);
  uniFullLFOPhaseOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullLFOPhaseOffset);
  uniFullLFOPhaseOffset.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullLFOPhaseOffset, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullLFOPhaseRand = result->params[(int)FFMasterParam::UniFullLFOPhaseRand];
  uniFullLFOPhaseRand.mode = FBParamMode::Block;
  uniFullLFOPhaseRand.defaultText = "0";
  uniFullLFOPhaseRand.name = "Uni LFO Phs Rnd";
  uniFullLFOPhaseRand.display = "LFO Phs Rnd";
  uniFullLFOPhaseRand.slotCount = FFMasterUniMaxCount;
  uniFullLFOPhaseRand.unit = "%";
  uniFullLFOPhaseRand.id = "{78DD7308-DA56-4A61-8940-6FE8F0632EAD}";
  uniFullLFOPhaseRand.type = FBParamType::Identity;
  auto selectUniFullLFOPhaseRand = [](auto& module) { return &module.block.uniFullLFOPhaseRand; };
  uniFullLFOPhaseRand.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullLFOPhaseRand);
  uniFullLFOPhaseRand.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullLFOPhaseRand);
  uniFullLFOPhaseRand.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullLFOPhaseRand);
  uniFullLFOPhaseRand.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullLFOPhaseRand, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullVFXParamA = result->params[(int)FFMasterParam::UniFullVFXParamA];
  uniFullVFXParamA.mode = FBParamMode::Accurate;
  uniFullVFXParamA.defaultText = "50";
  uniFullVFXParamA.name = "Uni VFX Param A";
  uniFullVFXParamA.display = "VFX Param A";
  uniFullVFXParamA.slotCount = FFMasterUniMaxCount;
  uniFullVFXParamA.unit = "%";
  uniFullVFXParamA.id = "{E383FFF3-6549-44A4-878B-9BE81EB4A69F}";
  uniFullVFXParamA.type = FBParamType::Identity;
  auto selectUniFullVFXParamA = [](auto& module) { return &module.acc.uniFullVFXParamA; };
  uniFullVFXParamA.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullVFXParamA);
  uniFullVFXParamA.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullVFXParamA);
  uniFullVFXParamA.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullVFXParamA);
  uniFullVFXParamA.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullVFXParamA, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullVFXParamB = result->params[(int)FFMasterParam::UniFullVFXParamB];
  uniFullVFXParamB.mode = FBParamMode::Accurate;
  uniFullVFXParamB.defaultText = "50";
  uniFullVFXParamB.name = "Uni VFX Param B";
  uniFullVFXParamB.display = "VFX Param B";
  uniFullVFXParamB.slotCount = FFMasterUniMaxCount;
  uniFullVFXParamB.unit = "%";
  uniFullVFXParamB.id = "{82D2D353-D454-496B-BB0E-F7B23D20D698}";
  uniFullVFXParamB.type = FBParamType::Identity;
  auto selectUniFullVFXParamB = [](auto& module) { return &module.acc.uniFullVFXParamB; };
  uniFullVFXParamB.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullVFXParamB);
  uniFullVFXParamB.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullVFXParamB);
  uniFullVFXParamB.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullVFXParamB);
  uniFullVFXParamB.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullVFXParamB, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullVFXParamC = result->params[(int)FFMasterParam::UniFullVFXParamC];
  uniFullVFXParamC.mode = FBParamMode::Accurate;
  uniFullVFXParamC.defaultText = "50";
  uniFullVFXParamC.name = "Uni VFX Param C";
  uniFullVFXParamC.display = "VFX Param C";
  uniFullVFXParamC.slotCount = FFMasterUniMaxCount;
  uniFullVFXParamC.unit = "%";
  uniFullVFXParamC.id = "{55539EFD-3561-4F1F-BDDC-FF0CEC2AA14B}";
  uniFullVFXParamC.type = FBParamType::Identity;
  auto selectUniFullVFXParamC = [](auto& module) { return &module.acc.uniFullVFXParamC; };
  uniFullVFXParamC.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullVFXParamC);
  uniFullVFXParamC.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullVFXParamC);
  uniFullVFXParamC.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullVFXParamC);
  uniFullVFXParamC.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullVFXParamC, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullVFXParamD = result->params[(int)FFMasterParam::UniFullVFXParamD];
  uniFullVFXParamD.mode = FBParamMode::Accurate;
  uniFullVFXParamD.defaultText = "50";
  uniFullVFXParamD.name = "Uni VFX Param D";
  uniFullVFXParamD.display = "VFX Param D";
  uniFullVFXParamD.slotCount = FFMasterUniMaxCount;
  uniFullVFXParamD.unit = "%";
  uniFullVFXParamD.id = "{9346FF6B-068F-4F36-A4CD-68A652CF1A2C}";
  uniFullVFXParamD.type = FBParamType::Identity;
  auto selectUniFullVFXParamD = [](auto& module) { return &module.acc.uniFullVFXParamD; };
  uniFullVFXParamD.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullVFXParamD);
  uniFullVFXParamD.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullVFXParamD);
  uniFullVFXParamD.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullVFXParamD);
  uniFullVFXParamD.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullVFXParamD, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEnvOffset = result->params[(int)FFMasterParam::UniFullEnvOffset];
  uniFullEnvOffset.mode = FBParamMode::Block;
  uniFullEnvOffset.defaultText = "0";
  uniFullEnvOffset.name = "Uni Env Offset";
  uniFullEnvOffset.display = "Env Offset";
  uniFullEnvOffset.slotCount = FFMasterUniMaxCount;
  uniFullEnvOffset.unit = "Sec";
  uniFullEnvOffset.id = "{AEDA998E-B0B7-4A78-8C52-F6B809AC5352}";
  uniFullEnvOffset.type = FBParamType::Linear;
  uniFullEnvOffset.Linear().min = 0.0f;
  uniFullEnvOffset.Linear().max = 0.0f;
  uniFullEnvOffset.Linear().editSkewFactor = 0.5f;
  auto selectUniFullEnvOffset = [](auto& module) { return &module.block.uniFullEnvOffset; };
  uniFullEnvOffset.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEnvOffset);
  uniFullEnvOffset.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEnvOffset);
  uniFullEnvOffset.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEnvOffset);
  uniFullEnvOffset.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEnvOffset, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEnvStretch = result->params[(int)FFMasterParam::UniFullEnvStretch];
  uniFullEnvStretch.mode = FBParamMode::Block;
  uniFullEnvStretch.defaultText = "100";
  uniFullEnvStretch.name = "Uni Env Stretch";
  uniFullEnvStretch.display = "Env Stretch";
  uniFullEnvStretch.slotCount = FFMasterUniMaxCount;
  uniFullEnvStretch.unit = "%";
  uniFullEnvStretch.id = "{6155B06D-D928-4EBF-9A52-99EAA3341D26}";
  uniFullEnvStretch.type = FBParamType::Linear;
  uniFullEnvStretch.Linear().min = 0.5f;
  uniFullEnvStretch.Linear().max = 1.5f;
  uniFullEnvStretch.Linear().displayMultiplier = 100.0f;
  auto selectUniFullEnvStretch = [](auto& module) { return &module.block.uniFullEnvStretch; };
  uniFullEnvStretch.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEnvStretch);
  uniFullEnvStretch.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEnvStretch);
  uniFullEnvStretch.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEnvStretch);
  uniFullEnvStretch.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEnvStretch, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoExtend = result->params[(int)FFMasterParam::UniFullEchoExtend];
  uniFullEchoExtend.mode = FBParamMode::Block;
  uniFullEchoExtend.defaultText = "50";
  uniFullEchoExtend.name = "Uni Echo Extend";
  uniFullEchoExtend.display = "Echo Extend";
  uniFullEchoExtend.slotCount = FFMasterUniMaxCount;
  uniFullEchoExtend.unit = "%";
  uniFullEchoExtend.id = "{B60645F1-F349-4911-AA11-218E188C0D1F}";
  uniFullEchoExtend.type = FBParamType::Identity;
  auto selectUniFullEchoExtend = [](auto& module) { return &module.block.uniFullEchoExtend; };
  uniFullEchoExtend.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoExtend);
  uniFullEchoExtend.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoExtend);
  uniFullEchoExtend.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoExtend);
  uniFullEchoExtend.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoExtend, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoFade = result->params[(int)FFMasterParam::UniFullEchoFade];
  uniFullEchoFade.mode = FBParamMode::Block;
  uniFullEchoFade.defaultText = "50";
  uniFullEchoFade.name = "Uni Echo Fade";
  uniFullEchoFade.display = "Echo Fade";
  uniFullEchoFade.slotCount = FFMasterUniMaxCount;
  uniFullEchoFade.unit = "%";
  uniFullEchoFade.id = "{A9F81A4A-1FB3-418B-86F5-8F49C77934B7}";
  uniFullEchoFade.type = FBParamType::Identity;
  auto selectUniFullEchoFade = [](auto& module) { return &module.block.uniFullEchoFade; };
  uniFullEchoFade.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoFade);
  uniFullEchoFade.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoFade);
  uniFullEchoFade.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoFade);
  uniFullEchoFade.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoFade, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoTapLevel = result->params[(int)FFMasterParam::UniFullEchoTapLevel];
  uniFullEchoTapLevel.mode = FBParamMode::Accurate;
  uniFullEchoTapLevel.defaultText = "50";
  uniFullEchoTapLevel.name = "Uni Echo Tap Lvl";
  uniFullEchoTapLevel.display = "Echo Tap Lvl";
  uniFullEchoTapLevel.slotCount = FFMasterUniMaxCount;
  uniFullEchoTapLevel.unit = "%";
  uniFullEchoTapLevel.id = "{A480C75C-3429-44A1-BAAF-44341E793F80}";
  uniFullEchoTapLevel.type = FBParamType::Identity;
  auto selectUniFullEchoTapLevel = [](auto& module) { return &module.acc.uniFullEchoTapLevel; };
  uniFullEchoTapLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoTapLevel);
  uniFullEchoTapLevel.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoTapLevel);
  uniFullEchoTapLevel.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoTapLevel);
  uniFullEchoTapLevel.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoTapLevel, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoTapDelay = result->params[(int)FFMasterParam::UniFullEchoTapDelay];
  uniFullEchoTapDelay.mode = FBParamMode::Accurate;
  uniFullEchoTapDelay.defaultText = "50";
  uniFullEchoTapDelay.name = "Uni Echo Tap Dly";
  uniFullEchoTapDelay.display = "Echo Tap Dly";
  uniFullEchoTapDelay.slotCount = FFMasterUniMaxCount;
  uniFullEchoTapDelay.unit = "%";
  uniFullEchoTapDelay.id = "{158E67F4-DD9C-4A2A-93B8-6914B8F9DCED}";
  uniFullEchoTapDelay.type = FBParamType::Identity;
  auto selectUniFullEchoTapDelay = [](auto& module) { return &module.acc.uniFullEchoTapDelay; };
  uniFullEchoTapDelay.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoTapDelay);
  uniFullEchoTapDelay.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoTapDelay);
  uniFullEchoTapDelay.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoTapDelay);
  uniFullEchoTapDelay.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoTapDelay, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoTapBal = result->params[(int)FFMasterParam::UniFullEchoTapBal];
  uniFullEchoTapBal.mode = FBParamMode::Accurate;
  uniFullEchoTapBal.defaultText = "50";
  uniFullEchoTapBal.name = "Uni Echo Tap Bal";
  uniFullEchoTapBal.display = "Echo Tap Bal";
  uniFullEchoTapBal.slotCount = FFMasterUniMaxCount;
  uniFullEchoTapBal.unit = "%";
  uniFullEchoTapBal.id = "{1E8EDB12-2E07-47DE-BB0C-39AC596896E0}";
  uniFullEchoTapBal.type = FBParamType::Identity;
  auto selectUniFullEchoTapBal = [](auto& module) { return &module.acc.uniFullEchoTapBal; };
  uniFullEchoTapBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoTapBal);
  uniFullEchoTapBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoTapBal);
  uniFullEchoTapBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoTapBal);
  uniFullEchoTapBal.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoTapBal, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoFdbkDelay = result->params[(int)FFMasterParam::UniFullEchoFdbkDelay];
  uniFullEchoFdbkDelay.mode = FBParamMode::Accurate;
  uniFullEchoFdbkDelay.defaultText = "50";
  uniFullEchoFdbkDelay.name = "Uni Echo Fdbk Dly";
  uniFullEchoFdbkDelay.display = "Echo Fdbk Dly";
  uniFullEchoFdbkDelay.slotCount = FFMasterUniMaxCount;
  uniFullEchoFdbkDelay.unit = "%";
  uniFullEchoFdbkDelay.id = "{6D51D872-4BFD-4F06-B202-BD971DB527FA}";
  uniFullEchoFdbkDelay.type = FBParamType::Identity;
  auto selectUniFullEchoFdbkDelay = [](auto& module) { return &module.acc.uniFullEchoFdbkDelay; };
  uniFullEchoFdbkDelay.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoFdbkDelay);
  uniFullEchoFdbkDelay.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoFdbkDelay);
  uniFullEchoFdbkDelay.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoFdbkDelay);
  uniFullEchoFdbkDelay.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoFdbkDelay, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoFdbkMix = result->params[(int)FFMasterParam::UniFullEchoFdbkMix];
  uniFullEchoFdbkMix.mode = FBParamMode::Accurate;
  uniFullEchoFdbkMix.defaultText = "50";
  uniFullEchoFdbkMix.name = "Uni Echo Fdbk Mix";
  uniFullEchoFdbkMix.display = "Echo Fdbk Mix";
  uniFullEchoFdbkMix.slotCount = FFMasterUniMaxCount;
  uniFullEchoFdbkMix.unit = "%";
  uniFullEchoFdbkMix.id = "{41E192B1-909D-41B6-93B5-9343460670A2}";
  uniFullEchoFdbkMix.type = FBParamType::Identity;
  auto selectUniFullEchoFdbkMix = [](auto& module) { return &module.acc.uniFullEchoFdbkMix; };
  uniFullEchoFdbkMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoFdbkMix);
  uniFullEchoFdbkMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoFdbkMix);
  uniFullEchoFdbkMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoFdbkMix);
  uniFullEchoFdbkMix.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoFdbkMix, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoFdbkAmt = result->params[(int)FFMasterParam::UniFullEchoFdbkAmt];
  uniFullEchoFdbkAmt.mode = FBParamMode::Accurate;
  uniFullEchoFdbkAmt.defaultText = "50";
  uniFullEchoFdbkAmt.name = "Uni Echo Fdbk Amt";
  uniFullEchoFdbkAmt.display = "Echo Fdbk Amt";
  uniFullEchoFdbkAmt.slotCount = FFMasterUniMaxCount;
  uniFullEchoFdbkAmt.unit = "%";
  uniFullEchoFdbkAmt.id = "{BF96F4F2-74C7-4BA8-8FE5-72320499849E}";
  uniFullEchoFdbkAmt.type = FBParamType::Identity;
  auto selectUniFullEchoFdbkAmt = [](auto& module) { return &module.acc.uniFullEchoFdbkAmt; };
  uniFullEchoFdbkAmt.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoFdbkAmt);
  uniFullEchoFdbkAmt.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoFdbkAmt);
  uniFullEchoFdbkAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoFdbkAmt);
  uniFullEchoFdbkAmt.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoFdbkAmt, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoReverbMix = result->params[(int)FFMasterParam::UniFullEchoReverbMix];
  uniFullEchoReverbMix.mode = FBParamMode::Accurate;
  uniFullEchoReverbMix.defaultText = "50";
  uniFullEchoReverbMix.name = "Uni Echo Rvrb Mix";
  uniFullEchoReverbMix.display = "Echo Rvrb Mix";
  uniFullEchoReverbMix.slotCount = FFMasterUniMaxCount;
  uniFullEchoReverbMix.unit = "%";
  uniFullEchoReverbMix.id = "{475A0982-1DEE-4636-B532-0D0FCD816DEC}";
  uniFullEchoReverbMix.type = FBParamType::Identity;
  auto selectUniFullEchoReverbMix = [](auto& module) { return &module.acc.uniFullEchoReverbMix; };
  uniFullEchoReverbMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoReverbMix);
  uniFullEchoReverbMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoReverbMix);
  uniFullEchoReverbMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoReverbMix);
  uniFullEchoReverbMix.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoReverbMix, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoReverbSize = result->params[(int)FFMasterParam::UniFullEchoReverbSize];
  uniFullEchoReverbSize.mode = FBParamMode::Accurate;
  uniFullEchoReverbSize.defaultText = "50";
  uniFullEchoReverbSize.name = "Uni Echo Rvrb Size";
  uniFullEchoReverbSize.display = "Echo Rvrb Size";
  uniFullEchoReverbSize.slotCount = FFMasterUniMaxCount;
  uniFullEchoReverbSize.unit = "%";
  uniFullEchoReverbSize.id = "{8E997A4E-BCF6-4599-9329-377E81F77B5F}";
  uniFullEchoReverbSize.type = FBParamType::Identity;
  auto selectUniFullEchoReverbSize = [](auto& module) { return &module.acc.uniFullEchoReverbSize; };
  uniFullEchoReverbSize.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoReverbSize);
  uniFullEchoReverbSize.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoReverbSize);
  uniFullEchoReverbSize.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoReverbSize);
  uniFullEchoReverbSize.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoReverbSize, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

  auto& uniFullEchoReverbDamp = result->params[(int)FFMasterParam::UniFullEchoReverbDamp];
  uniFullEchoReverbDamp.mode = FBParamMode::Accurate;
  uniFullEchoReverbDamp.defaultText = "50";
  uniFullEchoReverbDamp.name = "Uni Echo Rvrb Damp";
  uniFullEchoReverbDamp.display = "Echo Rvrb Damp";
  uniFullEchoReverbDamp.slotCount = FFMasterUniMaxCount;
  uniFullEchoReverbDamp.unit = "%";
  uniFullEchoReverbDamp.id = "{6E27DACE-DEA9-4168-8752-6BFB0B09002E}";
  uniFullEchoReverbDamp.type = FBParamType::Identity;
  auto selectUniFullEchoReverbDamp = [](auto& module) { return &module.acc.uniFullEchoReverbDamp; };
  uniFullEchoReverbDamp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniFullEchoReverbDamp);
  uniFullEchoReverbDamp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniFullEchoReverbDamp);
  uniFullEchoReverbDamp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniFullEchoReverbDamp);
  uniFullEchoReverbDamp.dependencies.enabled.audio.WhenSlots({ { (int)FFMasterParam::UniCount, -1 }, { (int)FFMasterParam::UniFullEchoReverbDamp, -1 }, { (int)FFMasterParam::UniType } }, [](auto const& slots, auto const& vs) { return slots[1] < vs[0] && vs[2] == (int)FFMasterUniType::Full; });

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