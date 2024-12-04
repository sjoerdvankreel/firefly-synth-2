#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

template <class T>
struct alignas(alignof(T)) FFPlugState;

class FBFixedAudioBlock;
typedef FFPlugState<FBProcParamState> FFProcState;

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
  void ProcessPlug(FBPlugInputBlock const& input, FBFixedAudioBlock& output) override;
};