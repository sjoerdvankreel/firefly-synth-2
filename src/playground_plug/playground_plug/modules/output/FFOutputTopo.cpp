#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/output/FFOutputTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

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
  voices.acc = false;
  voices.output = true;
  voices.defaultText = "0";
  voices.name = "Voices";
  voices.slotCount = 1;
  voices.id = "{54C6CAD4-FA5A-4395-8131-C768E0AD48CF}";
  voices.type = FBParamType::Discrete;
  voices.Discrete().valueCount = FBMaxVoices + 1;
  auto selectVoices = [](auto& module) { return &module.block.voices; };
  voices.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVoices);
  voices.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectVoices);
  voices.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVoices);

  return result;
}