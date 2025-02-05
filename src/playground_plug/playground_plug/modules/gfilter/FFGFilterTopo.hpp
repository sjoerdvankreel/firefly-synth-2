#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();

enum class FFGFilterParam { On, Type, Freq, Res, Gain, Count };
enum class FFGFilterType { LPF, BPF, HPF, BSF, APF, PEQ, BLL, LSH, HSH };