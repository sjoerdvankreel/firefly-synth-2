#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

struct FBRuntimeTopo;
struct FBPlugInputBlock;
struct FBPlugOutputBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBPlugProcessor);

  virtual void ProcessPreVoice(FBPlugInputBlock const& input) = 0;
  virtual void ProcessVoice(FBPlugInputBlock const& input, int voice, int releaseAt) = 0;
  virtual void ProcessPostVoice(FBPlugInputBlock const& input, FBPlugOutputBlock& output) = 0;

  virtual void LeaseVoices(FBPlugInputBlock const& input) = 0;
  virtual void InitOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* procState, float sampleRate) = 0;
};