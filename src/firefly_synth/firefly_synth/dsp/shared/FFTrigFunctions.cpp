#include <firefly_synth/dsp/shared/FFTrigFunctions.hpp>

std::string
FFTrigFunctionToString(FFTrigFunction fun)
{
  switch (fun)
  {
    case FFTrigFunction::FFTrigSin: return "Sin";
    case FFTrigFunction::FFTrigCos: return "Cos";
    case FFTrigFunction::FFTrigSin2: return "Sin2";
    case FFTrigFunction::FFTrigCos2: return "Cos2";
    case FFTrigFunction::FFTrigSinCos: return "SnCs";
    case FFTrigFunction::FFTrigCosSin: return "CsSn";
    case FFTrigFunction::FFTrigSin3: return "Sin3";
    case FFTrigFunction::FFTrigCos3: return "Cos3";
    case FFTrigFunction::FFTrigSn2Cs: return "S2Cs";
    case FFTrigFunction::FFTrigCs2Sn: return "C2Sn";
    case FFTrigFunction::FFTrigSnCs2: return "SnC2";
    case FFTrigFunction::FFTrigCsSn2: return "CsS2";
    case FFTrigFunction::FFTrigSnCsSn: return "SCS";
    case FFTrigFunction::FFTrigCsSnCs: return "CSC";
    default: FB_ASSERT(false); return {};
  }
}