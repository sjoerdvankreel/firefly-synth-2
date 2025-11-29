#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakePanicTopo();
enum class FFPanicParam { FlushAudioToggle, Count };