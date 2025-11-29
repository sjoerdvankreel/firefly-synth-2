#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeOutputTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Output";
  result->slotCount = 1;
  result->id = "{6BDA100B-8E99-4108-B38C-E5C6418EAF4B}";
  result->params.resize((int)FFOutputParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.output; });
  auto selectModule = [](auto& state) { return &state.global.output; };

  auto& voices = result->params[(int)FFOutputParam::Voices];
  voices.mode = FBParamMode::Output;
  voices.defaultText = "0";
  voices.name = "Voices";
  voices.unit = "%";
  voices.slotCount = 1;
  voices.id = "{54C6CAD4-FA5A-4395-8131-C768E0AD48CF}";
  voices.type = FBParamType::Discrete;
  voices.Discrete().valueCount = 101;
  auto selectVoices = [](auto& module) { return &module.block.voices; };
  voices.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoices);
  voices.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectVoices);
  voices.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoices);

  auto& cpu = result->params[(int)FFOutputParam::Cpu];
  cpu.mode = FBParamMode::Output;
  cpu.defaultText = "0";
  cpu.name = "CPU";
  cpu.unit = "%";
  cpu.slotCount = 1;
  cpu.id = "{F7E383FA-61A6-4CDB-898B-C62C248633A0}";
  cpu.type = FBParamType::Discrete;
  cpu.Discrete().valueCount = 101;
  auto selectCpu = [](auto& module) { return &module.block.cpu; };
  cpu.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCpu);
  cpu.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectCpu);
  cpu.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCpu);

  auto& gain = result->params[(int)FFOutputParam::Gain];
  gain.mode = FBParamMode::Output;
  gain.defaultText = "0";
  gain.name = "Gain";
  gain.unit = "%";
  gain.slotCount = 1;
  gain.id = "{F0F8F4CA-8F90-4C6D-B685-24119817CC1D}";
  gain.type = FBParamType::Discrete;
  gain.Discrete().valueCount = 101;
  auto selectGain = [](auto& module) { return &module.block.gain; };
  gain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectGain);
  gain.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGain);

  auto& mtsEspOn = result->params[(int)FFOutputParam::MtsEspOn];
  mtsEspOn.mode = FBParamMode::Output;
  mtsEspOn.name = "MTS-ESP";
  mtsEspOn.slotCount = 1;
  mtsEspOn.defaultText = "Off";
  mtsEspOn.id = "{9803DBBB-E1C9-4FAC-ACEA-B0E7F9E28979}";
  mtsEspOn.type = FBParamType::Boolean;
  auto selectMtsEspOn = [](auto& module) { return &module.block.mtsEspOn; };
  mtsEspOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectMtsEspOn);
  mtsEspOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectMtsEspOn);
  mtsEspOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectMtsEspOn);

  return result;
}