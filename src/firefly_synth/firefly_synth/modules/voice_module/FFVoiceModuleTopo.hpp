#pragma once

#include <memory>

struct FBStaticModule;

enum class FFVoiceModulePortaType { Off, On, Auto };
enum class FFVoiceModulePortaMode { Always, Release };
enum class FFVoiceModuleParam { 
  Coarse, Env5ToCoarse, Fine, LFO5ToFine, 
  PortaType, PortaMode, PortaSync, PortaTime, PortaBars, Count };
std::unique_ptr<FBStaticModule> FFMakeVoiceModuleTopo();
