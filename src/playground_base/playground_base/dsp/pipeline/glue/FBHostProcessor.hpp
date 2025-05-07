#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

struct FBRuntimeTopo;
struct FBHostInputBlock;
struct FBHostOutputBlock;

class FBVoiceManager;
class FBHostAudioBlock;
class IFBPlugProcessor;
class IFBHostDSPContext;
class FBSmoothingProcessor;
class FBProcStateContainer;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;
class FBExchangeStateContainer;

class FBHostProcessor final
{
  FBPlugInputBlock _plugIn = {};
  FBFixedOutputBlock _fixedOut = {};

  float const _sampleRate;
  FBRuntimeTopo const* const _topo;
  FBProcStateContainer* const _procState;
  FBExchangeStateContainer* const _exchangeState;
  std::unique_ptr<IFBPlugProcessor> _plug;
  std::unique_ptr<FBVoiceManager> _voiceManager;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;
  std::unique_ptr<FBSmoothingProcessor> _smoothing;

  void ProcessVoices();

public:
  ~FBHostProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostProcessor);
  FBHostProcessor(IFBHostDSPContext* hostContext);
  void ProcessHost(FBHostInputBlock const& input, FBHostOutputBlock& output);
};