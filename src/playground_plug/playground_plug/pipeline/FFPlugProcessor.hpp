#pragma once

#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugProcessor.hpp>

struct FFProcState;
struct FBRuntimeTopo;
struct FFExchangeState;
struct FBFixedOutputBlock;

class IFBHostDSPContext;
class FBFixedFloatAudioBlock;

class FFPlugProcessor final:
public IFBPlugProcessor
{
  float const _sampleRate;
  FBRuntimeTopo const* const _topo;
  FFProcState* const _procState;
  FFExchangeState* const _exchangeState;

  FBModuleProcState MakeModuleState(FBPlugInputBlock const& input) const;
  FBModuleProcState MakeModuleVoiceState(FBPlugInputBlock const& input, int voice) const;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(IFBHostDSPContext* hostContext);

  void LeaseVoices(FBPlugInputBlock const& input) override;
  void ProcessPreVoice(FBPlugInputBlock const& input) override;
  void ProcessVoice(FBPlugInputBlock const& input, int voice) override;
  void ProcessPostVoice(FBPlugInputBlock const& input, FBFixedOutputBlock& output) override;
};