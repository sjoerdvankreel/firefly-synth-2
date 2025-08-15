#pragma once

#include <firefly_synth/modules/midi/FFMIDIProcessor.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFMIDIDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFMIDIProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFMIDIDSPState);
  FFMIDIDSPState(): processor(std::make_unique<FFMIDIProcessor>()) {}
  FBSArray<float, FBFixedBlockSamples> outputCP = {};
  FBSArray<float, FBFixedBlockSamples> outputPB = {};
  FBSArray2<float, FBFixedBlockSamples, FBMIDIEvent::CCMessageCount> outputCC = {};
};