#pragma once

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBKeyMatrix.hpp>
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
  FBKeyMatrix<float> _keyMatrix = {};
  std::array<bool, 128> _keyOn = {};
  std::array<float, 128> _keyVelo = {};

  float const _sampleRate;
  float _prevRoundCpuUsage = 0.0f;
  FBRuntimeTopo const* const _topo;
  FBProcStateContainer* const _procState;
  FBExchangeStateContainer* const _exchangeState;
  std::unique_ptr<IFBPlugProcessor> _plug;
  std::unique_ptr<FBVoiceManager> _voiceManager;
  std::unique_ptr<FBHostToPlugProcessor> _hostToPlug;
  std::unique_ptr<FBPlugToHostProcessor> _plugToHost;
  std::unique_ptr<FBSmoothingProcessor> _smoothing;

  void UpdateKeyMatrix(FBNoteEvent const& event);

public:
  ~FBHostProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostProcessor);
  FBHostProcessor(IFBHostDSPContext* hostContext);
  void ProcessHost(FBHostInputBlock const& input, FBHostOutputBlock& output);
};