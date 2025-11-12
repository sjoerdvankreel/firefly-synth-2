#pragma once

#include <memory>

struct FBStaticModule;

enum class FFGNoteCVOutput { NoteKeyMatrixRaw, NoteKeyMatrixSmth, NoteVeloMatrixRaw, NoteVeloMatrixSmth, Count };
std::unique_ptr<FBStaticModule> FFMakeGNoteTopo();