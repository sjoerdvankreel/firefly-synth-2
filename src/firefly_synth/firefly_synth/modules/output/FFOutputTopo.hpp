#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOutputTopo();
enum class FFOutputParam { Voices, Cpu, Gain, MtsEspOn, Count };