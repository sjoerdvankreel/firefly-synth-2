#pragma once

#include <playground_base/dsp/host/FBHostInputBlock.hpp>
#include <public.sdk/source/vst/vstaudioeffect.h>

#include <array>
#include <vector>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBRuntimeTopo;
class FBHostProcessor;
class IFBPlugProcessor;

class FBVST3AudioEffect :
public AudioEffect
{
  FBHostInputBlock _input = {};
  std::unique_ptr<FBRuntimeTopo> _topo;
  std::array<std::vector<float>, 2> _zeroIn = {};
  std::unique_ptr<FBHostProcessor> _hostProcessor = {};

protected:
  virtual std::unique_ptr<IFBPlugProcessor> 
  MakePlugProcessor(
    FBRuntimeTopo const& topo, float sampleRate) const = 0;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3AudioEffect);
  FBVST3AudioEffect(std::unique_ptr<FBRuntimeTopo>&& topo, FUID const& controllerId);

  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSize) override;

  tresult PLUGIN_API setBusArrangements(
    SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
};