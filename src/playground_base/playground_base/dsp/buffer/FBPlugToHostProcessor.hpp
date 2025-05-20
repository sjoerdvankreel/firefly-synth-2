#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/host/FBHostBlock.hpp>
#include <playground_base/dsp/buffer/FBBufferBlock.hpp>

#include <memory>

class FBVoiceManager;
class FBHostAudioBlock;

class FBPlugToHostProcessor final
{
  FBBufferAudioBlock _buffer = {};
  std::vector<FBNote> _returnedVoices = {};
  FBVoiceManager* _voiceManager;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugToHostProcessor);
  FBPlugToHostProcessor(FBVoiceManager* voiceManager);

  void ProcessToHost(FBHostOutputBlock& hostBlock);
  void BufferFromPlug(FBArray2<float, FBFixedBlockSamples, 2> const& plugAudio);
};