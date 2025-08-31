#pragma once

#include <firefly_synth/modules/note/FFGNoteProcessor.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/shared/FBKeyMatrix.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFGNoteDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFGNoteProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGNoteDSPState);
  FFGNoteDSPState(): processor(std::make_unique<FFGNoteProcessor>()) {}
  FBKeyMatrix<FBSArray<float, FBFixedBlockSamples>> outputKeyMatrix = {};
};