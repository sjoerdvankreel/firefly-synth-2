#pragma once

#include <memory>

struct FBStaticModule;

// TODO tuned versions ?
enum class FFGNoteCVOutput { NoteMatrixRaw, Count };
std::unique_ptr<FBStaticModule> FFMakeGNoteTopo();