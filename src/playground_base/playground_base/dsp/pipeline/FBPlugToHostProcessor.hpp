#pragma once

#include <playground_base/base/shared/FBObjectLifetime.hpp>
#include <memory>

class FBHostAudioBlock;
class FBPlugAudioBlock;
class FBPipelineAudioBlock;

class FBPlugToHostProcessor
{
  bool _paddedOnce = false;
  bool _hitPlugBlockSize = false;
  std::unique_ptr<FBPipelineAudioBlock> _pipeline = {};

public:
  void ToHost(FBHostAudioBlock& host);
  void FromPlug(FBPlugAudioBlock const& plug);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugToHostProcessor);
};