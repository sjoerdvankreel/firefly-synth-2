#pragma once

#include <memory>

struct FBStaticModule;
inline int constexpr FFVNoteOnNoteRandomCount = 3;
std::unique_ptr<FBStaticModule> FFMakeVNoteTopo();

enum class FFVNoteCVOutput { 
  Key, Velo, 
  RandomUni, RandomNorm, 
  GroupRandomUni, GroupRandomNorm, // global unison
  Count };