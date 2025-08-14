#pragma once

#include <memory>

struct FBStaticModule;
enum class FFMIDICVOutput { CP, PB, CC, Count };
std::unique_ptr<FBStaticModule> FFMakeMIDITopo();