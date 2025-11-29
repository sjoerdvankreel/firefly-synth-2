#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/panic/FFPanicTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakePanicTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Panic";
  result->slotCount = 1;
  result->id = "{FE23D5B0-D854-4A62-BF43-7D583C17AAD3}";
  result->params.resize((int)FFPanicParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.panic; });
  auto selectModule = [](auto& state) { return &state.global.panic; };

  // dummy which we check on the audio if it changed
  auto& flushAudioToggle = result->params[(int)FFPanicParam::FlushAudioToggle];
  flushAudioToggle.mode = FBParamMode::Fake;
  flushAudioToggle.name = "Flush Audio";
  flushAudioToggle.slotCount = 1;
  flushAudioToggle.defaultText = "Off";
  flushAudioToggle.id = "{BF11CEEB-B041-4133-A93E-CA4C43E139D6}";
  flushAudioToggle.type = FBParamType::Boolean;
  auto selectFlushAudioToggle = [](auto& module) { return &module.block.flushAudioToggle; };
  flushAudioToggle.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFlushAudioToggle);
  flushAudioToggle.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFlushAudioToggle);
  flushAudioToggle.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFlushAudioToggle);

  return result;
}