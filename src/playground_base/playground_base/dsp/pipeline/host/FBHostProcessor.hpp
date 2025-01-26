#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

struct FBRuntimeTopo;
struct FBHostInputBlock;
struct FBHostOutputBlock;

class FBVoiceManager;
class FBHostAudioBlock;
class IFBPlugProcessor;
class FBSmoothingProcessor;
class FBProcStateContainer;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;

class FBHostProcessor final
{
  float _sampleRate;
  FBRuntimeTopo const* _topo;
  FBFixedOutputBlock _fixedOut;
  FBPlugInputBlock _plugIn = {};
  FBProcStateContainer* _state = {};
  std::unique_ptr<IFBPlugProcessor> _plug;
  std::unique_ptr<FBVoiceManager> _voiceManager;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;
  std::unique_ptr<FBSmoothingProcessor> _smoothing;

  void ProcessVoices();

public:
  FBHostProcessor(
    FBRuntimeTopo const* topo,
    std::unique_ptr<IFBPlugProcessor>&& plug,
    FBProcStateContainer* state, float sampleRate);

  ~FBHostProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostProcessor);
  void ProcessHost(FBHostInputBlock const& input, FBHostOutputBlock& output);
};