#pragma once

#include <memory>

struct FBStaticModule;

enum class FFGNoteCVOutput { NoteMatrixRaw, NoteMatrixSmth, Count };
std::unique_ptr<FBStaticModule> FFMakeGNoteTopo();