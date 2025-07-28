#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/topo/static/FBSlotFormatter.hpp>

#include <string>
#include <functional>

typedef std::function<FBSArray<float, FBFixedBlockSamples>* (
int moduleSlot, int cvOutputSlot, void* state)>
FBGlobalProcCVOutputAddrSelector;
typedef std::function<FBSArray<float, FBFixedBlockSamples>* (
int moduleSlot, int cvOutputSlot, int voice, void* state)>
FBVoiceProcCVOutputAddrSelector;

struct FBStaticCVOutput final
{
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  FBSlotFormatter slotFormatter = {};

  FBVoiceProcCVOutputAddrSelector voiceAddr = {};
  FBGlobalProcCVOutputAddrSelector globalAddr = {};
};