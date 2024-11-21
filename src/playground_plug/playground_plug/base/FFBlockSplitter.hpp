#pragma once

#include <playground_plug/base/FFDSPBlock.hpp>
#include <playground_plug/base/FFHostBlock.hpp>
#include <playground_plug/base/FFDSPConfig.hpp>

class FFBlockSplitter
{
  int _accumulatedSampleCount;
  FFHostBlock _accumulatedBlock;

public:
  FFBlockSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFBlockSplitter);

  void RemoveOneFixedBlock();
  bool GetCurrentFixedBlock(FFHostBlock const* fixedBlock);
  void Accumulate(FFHostBlock const& hostBlock, int currentHostSampleCount);
};