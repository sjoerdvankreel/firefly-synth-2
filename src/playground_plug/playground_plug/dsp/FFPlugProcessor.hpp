#pragma once

#include <playground_plug/dsp/FFModuleProcState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

struct FFProcState;
class FBFixedAudioBlock;

class FFPlugProcessor final:
public IFBPlugProcessor
{
  FBStaticTopo _topo;
  FFProcState* _state;
  float const _sampleRate;

  FFModuleProcState MakeModuleState(FBPlugInputBlock const& input) const;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(FBStaticTopo const& topo, FFProcState* state, float sampleRate);

  void ProcessPreVoice(FBPlugInputBlock const& input) override;
  void ProcessVoice(FBPlugInputBlock const& input, int voice) override;
  void ProcessPostVoice(FBPlugInputBlock const& input, FBFixedAudioBlock& output) override;
};