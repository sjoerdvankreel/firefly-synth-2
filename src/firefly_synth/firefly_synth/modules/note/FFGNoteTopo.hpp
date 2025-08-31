#pragma once

#include <memory>

struct FBStaticModule;

// TODO tuned versions ?
enum class FFGNoteCVOutput {
  LastKeyRaw, LastKeySmth, LastVeloRaw, LastVeloSmth,
  LowKeyKeyRaw, LowKeyKeySmth, LowKeyVeloRaw, LowKeyVeloSmth,
  HighKeyKeyRaw, HighKeyKeySmth, HighKeyVeloRaw, HighKeyVeloSmth,
  LowVeloKeyRaw, LowVeloKeySmth, LowVeloVeloRaw, LowVeloVeloSmth,
  HighVeloKeyRaw, HighVeloKeySmth, HighVeloVeloRaw, HighVeloVeloSmth,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGNoteTopo();