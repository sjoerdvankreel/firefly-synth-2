#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();

enum class FFGFilterParam { On, Mode, Freq, Res, Gain, Count };
enum class FFGFilterMode { LPF, BPF, HPF, BSF, APF, PEQ, BLL, LSH, HSH };