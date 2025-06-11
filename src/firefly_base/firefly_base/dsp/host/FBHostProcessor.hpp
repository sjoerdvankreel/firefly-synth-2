#pragma once

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

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
class FBHostToPlugProcessor;
class FBPlugToHostProcessor;
class FBExchangeStateContainer;

class FBHostProcessor final
{
  FBPlugInputBlock _plugIn = {};
  FBPlugOutputBlock _plugOut = {};

  float const _sampleRate;
  float _lastNotePrevBlock;
  FBRuntimeTopo const* const _topo;
  FBProcStateContainer* const _procState;
  FBExchangeStateContainer* const _exchangeState;
  std::unique_ptr<IFBPlugProcessor> _plug;
  std::unique_ptr<FBVoiceManager> _voiceManager;
  std::unique_ptr<FBSmoothingProcessor> _smoothing;
  std::unique_ptr<FBHostToPlugProcessor> _hostToPlug;
  std::unique_ptr<FBPlugToHostProcessor> _plugToHost;

  void ProcessVoices();

public:
  ~FBHostProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostProcessor);
  FBHostProcessor(IFBHostDSPContext* hostContext);
  void ProcessHost(FBHostInputBlock const& input, FBHostOutputBlock& output);
};