#pragma once

#include <memory>

struct FBStaticModule;
enum class FFVNoteCVOutput { KeyUntuned, Velo, Count };
std::unique_ptr<FBStaticModule> FFMakeVNoteTopo();