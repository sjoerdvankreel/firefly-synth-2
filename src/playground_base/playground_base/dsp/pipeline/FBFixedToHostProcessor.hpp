#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <memory>

class FBHostAudioBlock;
class FBFixedAudioBlock;
class FBPipelineAudioBlock;

class FBFixedToHostProcessor
{
  bool _paddedOnce = false;
  bool _hitFixedBlockSize = false;
  std::unique_ptr<FBPipelineAudioBlock> _pipeline = {};

public:
  void ToHost(FBHostAudioBlock& host);
  void FromFixed(FBFixedAudioBlock const& fixed);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedToHostProcessor);
};