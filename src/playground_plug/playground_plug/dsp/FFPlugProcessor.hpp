#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

struct FFProcState;
class FBFixedAudioBlock;

class FFPlugProcessor final:
public IFBPlugProcessor
{
  float _phase = 0;
  FBStaticTopo _topo;
  FFProcState* _state;
  float const _sampleRate;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(FBStaticTopo const& topo, FFProcState* state, float sampleRate);
  void ProcessPlug(FBFixedAudioBlock const& input, FBFixedAudioBlock& output) override;
};