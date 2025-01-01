#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();

enum { FFGFilterBlockOn, FFGFilterBlockType, FFGFilterAccFreq, FFGFilterAccRes, FFGFilterParamCount };
enum { FFGFilterTypeLPF, FFGFilterTypeBPF, FFGFilterTypeHPF, FFGFilterTypeBSF, FFGFilterTypeAPF, FFGFilterTypePEQ, FFGFilterTypeBLL, FFGFilterTypeLSH, FFGFilterTypeHSH, FFGFilterTypeCount };