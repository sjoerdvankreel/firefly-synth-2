#pragma once

#include <firefly_synth/modules/note/FFVNoteTopo.hpp>
#include <firefly_synth/modules/note/FFVNoteProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFVNoteDSPState final
{
  friend class FFVoiceProcessor;
  std::unique_ptr<FFVNoteProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFVNoteDSPState);
  FFVNoteDSPState(): processor(std::make_unique<FFVNoteProcessor>()) {}
  FBSArray<float, FBFixedBlockSamples> outputVelo = {};
  FBSArray<float, FBFixedBlockSamples> outputKeyUntuned = {};
  FBSArray2<float, FBFixedBlockSamples, FFVNoteOnNoteRandomCount> outputRandomUni = {};
  FBSArray2<float, FBFixedBlockSamples, FFVNoteOnNoteRandomCount> outputRandomNorm = {};
  FBSArray2<float, FBFixedBlockSamples, FFVNoteOnNoteRandomCount> outputGroupRandomUni = {};
  FBSArray2<float, FBFixedBlockSamples, FFVNoteOnNoteRandomCount> outputGroupRandomNorm = {};
};