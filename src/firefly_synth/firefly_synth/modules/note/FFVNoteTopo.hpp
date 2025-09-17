#pragma once

#include <memory>

struct FBStaticModule;
inline int constexpr FFVNoteOnNoteRandomCount = 3;
enum class FFVNoteCVOutput { KeyUntuned, Velo, RandomUni, RandomNorm, Count };

std::unique_ptr<FBStaticModule> FFMakeVNoteTopo();