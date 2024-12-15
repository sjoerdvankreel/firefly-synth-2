#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

struct FBRuntimeTopo;
struct FBHostInputBlock;

class FBVoiceManager;
class FBProcStatePtrs;
class FBHostAudioBlock;
class IFBPlugProcessor;
class FBSmoothProcessor;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;

class FBHostProcessor final
{
  float _sampleRate = {};
  FBProcStatePtrs* _state = {};
  FBPlugInputBlock _plugIn = {};
  FBFixedOutputBlock _fixedOut = {};

  std::unique_ptr<IFBPlugProcessor> _plug;
  std::unique_ptr<FBVoiceManager> _voiceManager;
  std::unique_ptr<FBSmoothProcessor> _smooth;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;

  void ProcessVoices();

public:
  ~FBHostProcessor();  
  FB_NOCOPY_MOVE_NODEFCTOR(FBHostProcessor);

  FBHostProcessor(
    std::unique_ptr<IFBPlugProcessor>&& plug,
    FBProcStatePtrs* state, float sampleRate);
  void ProcessHost(FBHostInputBlock const& input, FBHostAudioBlock& output);
};