#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
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

  void ProcessVoice(int voice) override;
  void ProcessPreVoice(FBPlugInputBlock const& input) override;
  void ProcessPostVoice(FBPlugInputBlock const& input, FBFixedAudioBlock& output) override;
};