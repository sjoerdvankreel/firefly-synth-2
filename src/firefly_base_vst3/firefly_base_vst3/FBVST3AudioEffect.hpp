#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/host/FBHostDSPContext.hpp>

#include <public.sdk/source/vst/vstaudioeffect.h>
#include <public.sdk/source/vst/utility/dataexchange.h>

#include <array>
#include <vector>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBStaticTopo;
struct FBRuntimeTopo;

class MTSClient;
class FBHostProcessor;
class IFBPlugProcessor;
class FBProcStateContainer;
class FBExchangeStateContainer;

class FBVST3AudioEffect:
public AudioEffect,
public IFBHostDSPContext
{
  MTSClient* _mtsClient;
  std::unique_ptr<FBRuntimeTopo> _topo;
  std::unique_ptr<FBProcStateContainer> _procState;
  std::unique_ptr<FBExchangeStateContainer> _exchangeState;

  float _sampleRate = 0.0f;
  FBHostInputBlock _input = {};
  FBHostOutputBlock _output = {};
  std::array<std::vector<float>, 2> _zeroIn = {};
  std::unique_ptr<FBHostProcessor> _hostProcessor = {};
  std::unique_ptr<DataExchangeHandler> _exchangeHandler = {};
  DataExchangeBlock _exchangeBlock = { nullptr, 0, InvalidDataExchangeBlockID };

public:
  ~FBVST3AudioEffect();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3AudioEffect);
  FBVST3AudioEffect(std::unique_ptr<FBStaticTopo>&& topo, FUID const& controllerId, MTSClient* mtsClient);
  uint32 PLUGIN_API getLatencySamples() override { return FBFixedBlockSamples; }

  float SampleRate() const override { return _sampleRate; }
  MTSClient* GetMTSClient() override { return _mtsClient; }
  FBRuntimeTopo const* Topo() const override { return _topo.get(); }
  FBProcStateContainer* ProcState() override { return _procState.get(); }
  FBExchangeStateContainer* ExchangeState() override { return _exchangeState.get(); }

  tresult PLUGIN_API setActive(TBool state) override;
  tresult PLUGIN_API setState(IBStream* state) override;
  tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API connect(IConnectionPoint* other) override;
  tresult PLUGIN_API disconnect(IConnectionPoint* other) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSize) override;
  tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
};