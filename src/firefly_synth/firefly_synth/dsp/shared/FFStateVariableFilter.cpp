#include <firefly_synth/dsp/shared/FFStateVariableFilter.hpp>

std::string
FFStateVariableFilterModeToString(FFStateVariableFilterMode mode)
{
  switch (mode)
  {
  case FFStateVariableFilterMode::LPF: return "LPF";
  case FFStateVariableFilterMode::BPF: return "BPF";
  case FFStateVariableFilterMode::HPF: return "HPF";
  case FFStateVariableFilterMode::BSF: return "BSF";
  case FFStateVariableFilterMode::APF: return "APF";
  case FFStateVariableFilterMode::PEQ: return "PEQ";
  case FFStateVariableFilterMode::BLL: return "BLL";
  case FFStateVariableFilterMode::LSH: return "LSH";
  case FFStateVariableFilterMode::HSH: return "HSH";
  default: FB_ASSERT(false); return {};
  }
}