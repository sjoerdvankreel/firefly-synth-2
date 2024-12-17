#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

#include <memory>

class FBVoiceManager;
class FBHostAudioBlock;
class FBFixedAudioBlock;

class FBFixedBufferProcessor final
{
  bool _paddedOnce = false;
  bool _hitFixedBlockSize = false;
  FBBufferAudioBlock _buffer = {};
  std::vector<FBNote> _returnedVoices = {};
  FBVoiceManager* _voiceManager;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBufferProcessor);
  FBFixedBufferProcessor(FBVoiceManager* voiceManager);

  void ProcessToHost(FBHostOutputBlock& host);
  void BufferFromFixed(FBFixedAudioBlock const& fixed);
};