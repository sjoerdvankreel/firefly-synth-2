#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOtherTopo();
enum class FFOtherParam { FlushAudioToggle, Count };