#include <firefly_synth/dsp/shared/FFModulate.hpp>

std::string
FFModulationOpTypeToString(FFModulationOpType opType)
{
  switch (opType)
  {
  case FFModulationOpType::Off: return "Off";
  case FFModulationOpType::UPAddU: return "UP AdU";
  case FFModulationOpType::UPAddB: return "UP AdB";
  case FFModulationOpType::UPMul: return "UP Mul";
  case FFModulationOpType::UPStack: return "UP Stk";
  case FFModulationOpType::UPRemap: return "UP Rmp";
  case FFModulationOpType::BPAddB: return "BP AdB";
  case FFModulationOpType::BPAddU: return "BP AdU";
  case FFModulationOpType::BPMul: return "BP Mul";
  case FFModulationOpType::BPStack: return "BP Stk";
  case FFModulationOpType::BPRemap: return "BP Rmp";
  case FFModulationOpType::PhaseWrap: return "Ph Wrp";
  default: FB_ASSERT(false); return "";
  }
}