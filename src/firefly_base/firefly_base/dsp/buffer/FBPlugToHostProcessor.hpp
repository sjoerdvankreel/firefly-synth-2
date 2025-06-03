#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/buffer/FBBufferBlock.hpp>

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
  void BufferFromPlug(FBSArray2<float, FBFixedBlockSamples, 2> const& plugAudio);
};