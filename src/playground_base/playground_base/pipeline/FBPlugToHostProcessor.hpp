#pragma once
#include <playground_base/shared/FBObjectLifetime.hpp>
#include <memory>

class FBHostAudioBlock;
class FBPlugAudioBlock;
class FBPipelineAudioBlock;

class FBPlugToHostProcessor
{
  bool _hitPlugBlockSize;
  std::unique_ptr<FBPipelineAudioBlock> _pipeline;

public:
  void ToHost(FBHostAudioBlock& host);
  void FromPlug(FBPlugAudioBlock const& plug);

  FBPlugToHostProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugToHostProcessor);
};