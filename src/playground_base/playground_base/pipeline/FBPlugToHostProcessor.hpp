#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBHostAudioBlock;

class FBPlugToHostProcessor
{
  bool _hitFixedBlockSize;
  FBPipelineAudioBlock _pipeline;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugToHostProcessor);
  void ToHost(FBHostAudioBlock& output);
  void FromPlug(FBPlugAudioBlock const& input);
};