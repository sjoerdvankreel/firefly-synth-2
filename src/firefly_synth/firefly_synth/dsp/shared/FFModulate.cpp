#include <firefly_synth/dsp/shared/FFModulate.hpp>

std::string
FFModulationOpTypeToString(FFModulationOpType opType)
{
  switch (opType)
  {
  case FFModulationOpType::Off: return "Off";
  case FFModulationOpType::UPAddU: return "UP Add UP";
  case FFModulationOpType::UPAddB: return "UP Add BP";
  case FFModulationOpType::UPMul: return "UP Mul";
  case FFModulationOpType::UPStack: return "UP Stack";
  case FFModulationOpType::UPRemap: return "UP Remap";
  case FFModulationOpType::BPAddB: return "BP Add BP";
  case FFModulationOpType::BPAddU: return "BP Add UP";
  case FFModulationOpType::BPMul: return "BP Mul";
  case FFModulationOpType::BPStack: return "BP Stack";
  case FFModulationOpType::BPRemap: return "BP Remap";
  case FFModulationOpType::PhaseWrap: return "Phs Wrap";
  default: FB_ASSERT(false); return "";
  }
}