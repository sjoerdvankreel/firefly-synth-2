#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();

enum class FFGLFOParam { On, Rate, Count };