#pragma once

#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

enum class FFModuleType { GLFO, GFilter, Osci, OsciMod, Env, Master, Output, GUISettings, Count }; 

inline int constexpr FFEnvCount = 8;
inline int constexpr FFOsciCount = 4;
inline int constexpr FFGLFOCount = 1;
inline int constexpr FFGFilterCount = 2;