#pragma once

#include <memory>

struct FBStaticModule;

// TODO tuned versions ?
enum class FFGNoteCVOutput { NoteMatrix, Count };
std::unique_ptr<FBStaticModule> FFMakeGNoteTopo();