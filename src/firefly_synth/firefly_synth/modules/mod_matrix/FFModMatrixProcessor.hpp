#pragma once

#include <firefly_synth/modules/mod_matrix/FFModMatrixProcessor.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFModMatrixProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixProcessor);

  template <bool Global>
  void Process(FBModuleProcState& state);
  template <bool Global>
  void BeginVoiceOrBlock(FBModuleProcState& state);
};