#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/host/FBHostBlock.hpp>
#include <playground_base/dsp/buffer/FBBufferBlock.hpp>

#include <memory>

class FBVoiceManager;
class FBHostAudioBlock;

class FBFixedBufferProcessor final
{
  FBBufferAudioBlock _buffer = {};
  std::vector<FBNote> _returnedVoices = {};
  FBVoiceManager* _voiceManager;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBufferProcessor);
  FBFixedBufferProcessor(FBVoiceManager* voiceManager);

  void ProcessToHost(FBHostOutputBlock& host);
  void BufferFromFixed(FBSIMDArray2<float, FBFixedBlockSamples, 2> const& fixed);
};