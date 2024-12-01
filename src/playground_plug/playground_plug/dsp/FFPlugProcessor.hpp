#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_base/dsp/plug/FBPlugProcessor.hpp>

struct FBRuntimeTopo;
class FBFixedAudioBlock;

class FFPlugProcessor:
public IFBPlugProcessor
{
  float _phase = 0;
  float const _sampleRate;
  FFProcMemory _memory = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(FBRuntimeTopo const& topo, float sampleRate);

  FBStateAddrs StateAddrs() override;
  void ProcessPlug(FBFixedAudioBlock const& input, FBFixedAudioBlock& output) override;
};