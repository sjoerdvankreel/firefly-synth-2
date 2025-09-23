#pragma once

#include <memory>

struct FBStaticModule;
inline int constexpr FFVNoteOnNoteRandomCount = 3;
enum class FFVNoteCVOutput { 
  KeyUntuned, Velo, 
  RandomUni, RandomNorm, 
  GroupRandomUni, GroupRandomNorm, // global unison
  Count };

std::unique_ptr<FBStaticModule> FFMakeVNoteTopo();