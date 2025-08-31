#pragma once

#include <firefly_synth/modules/note/FFGNoteProcessor.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>

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
  FBSArray<float, FBFixedBlockSamples> outputLastKeyRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputLastKeySmth = {};
  FBSArray<float, FBFixedBlockSamples> outputLastVeloRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputLastVeloSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputLowKeyKeyRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputLowKeyKeySmth = {};
  FBSArray<float, FBFixedBlockSamples> outputLowKeyVeloRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputLowKeyVeloSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputHighKeyKeyRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputHighKeyKeySmth = {};
  FBSArray<float, FBFixedBlockSamples> outputHighKeyVeloRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputHighKeyVeloSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputLowVeloKeyRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputLowVeloKeySmth = {};
  FBSArray<float, FBFixedBlockSamples> outputLowVeloVeloRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputLowVeloVeloSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputHighVeloKeyRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputHighVeloKeySmth = {};
  FBSArray<float, FBFixedBlockSamples> outputHighVeloVeloRaw = {};
  FBSArray<float, FBFixedBlockSamples> outputHighVeloVeloSmth = {};
};