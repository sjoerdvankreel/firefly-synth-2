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
  bendRange.display = "PB Range";
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
  bendTarget.display = "PB Target";
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