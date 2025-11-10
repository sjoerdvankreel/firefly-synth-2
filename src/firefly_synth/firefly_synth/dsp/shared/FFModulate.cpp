#include <firefly_synth/dsp/shared/FFModulate.hpp>

std::string
FFModulationOpTypeToString(FFModulationOpType opType)
{
  switch (opType)
  {
  case FFModulationOpType::Off: return "Off";
  case FFModulationOpType::UPAdd: return "UP Add";
  case FFModulationOpType::UPAdd2: return "UP Ad2";
  case FFModulationOpType::UPMul: return "UP Mul";
  case FFModulationOpType::UPStack: return "UP Stk";
  case FFModulationOpType::UPRemap: return "UP Rmp";
  case FFModulationOpType::BPAdd: return "BP Add";
  case FFModulationOpType::BPAdd2: return "BP Ad2";
  case FFModulationOpType::BPMul: return "BP Mul";
  case FFModulationOpType::BPStack: return "BP Stk";
  case FFModulationOpType::BPRemap: return "BP Rmp";
  case FFModulationOpType::PhaseWrap: return "Ph Wrp";
  default: FB_ASSERT(false); return "";
  }
}