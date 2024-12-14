#pragma once

#include <memory>

struct FBStaticModule;

inline float constexpr FFGLFORateMinHz = 0.1f;
inline float constexpr FFGLFORateMaxHz = 20.0f;

std::unique_ptr<FBStaticModule>
FFMakeGLFOTopo();