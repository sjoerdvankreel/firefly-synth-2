#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

struct FBRuntimeTopo;
struct FBHostInputBlock;
struct FBHostOutputBlock;

class IFBPlugProcessor;
class IFBHostProcessContext;

class FBVoiceManager;
class FBHostAudioBlock;
class FBSmoothProcessor;
class FBProcStateContainer;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;

class FBHostProcessor final
{
  float _sampleRate = {};
  FBPlugInputBlock _plugIn = {};
  FBFixedOutputBlock _fixedOut = {};
  FBProcStateContainer* _state = {};
  IFBHostProcessContext* _hostContext = {};

  std::unique_ptr<IFBPlugProcessor> _plug;
  std::unique_ptr<FBVoiceManager> _voiceManager;
  std::unique_ptr<FBSmoothProcessor> _smooth;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;

public:
  ~FBHostProcessor();  
  FB_NOCOPY_MOVE_NODEFCTOR(FBHostProcessor);

  FBHostProcessor(
    IFBHostProcessContext* hostContext,
    std::unique_ptr<IFBPlugProcessor>&& plug,
    FBProcStateContainer* state, float sampleRate);

  void ProcessAllVoices();
  void ProcessVoice(int slot);
  void ProcessHost(FBHostInputBlock const& input, FBHostOutputBlock& output);
};