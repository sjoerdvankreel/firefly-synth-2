#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

class FBHostAudioBlock;
struct FBHostInputBlock;

class FBRampProcessor;
class IFBPlugProcessor;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;

class FBHostProcessor
{
  FBFixedOutputBlock _fixedOut = {};
  std::unique_ptr<IFBPlugProcessor> _plug;

  std::unique_ptr<FBRampProcessor> _ramp;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;

public:
  ~FBHostProcessor();
  FB_NOCOPY_MOVE_NODEFCTOR(FBHostProcessor);

  FBHostProcessor(std::unique_ptr<IFBPlugProcessor>&& plug);
  void ProcessHost(FBHostInputBlock const& input, FBHostAudioBlock& output);
};