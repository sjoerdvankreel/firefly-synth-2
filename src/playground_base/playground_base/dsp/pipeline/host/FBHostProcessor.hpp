#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

struct FBRuntimeTopo;
struct FBHostInputBlock;
class FBHostAudioBlock;

class FBRampProcessor;
class FBSmoothProcessor;
class IFBPlugProcessor;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;

class FBHostProcessor final
{
  FBFixedOutputBlock _fixedOut = {};
  std::unique_ptr<IFBPlugProcessor> _plug;

  std::unique_ptr<FBRampProcessor> _ramp;
  std::unique_ptr<FBSmoothProcessor> _smooth;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;

public:
  ~FBHostProcessor();
  FB_NOCOPY_MOVE_NODEFCTOR(FBHostProcessor);
  void ProcessHost(FBHostInputBlock const& input, FBHostAudioBlock& output);
  FBHostProcessor(FBRuntimeTopo const& topo, std::unique_ptr<IFBPlugProcessor>&& plug, float sampleRate);
};