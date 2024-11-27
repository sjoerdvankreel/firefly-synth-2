#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBHostAudioBlock;

class FBPlugToHostProcessor
{
  bool _hitPlugBlockSize = false;
  FBPipelineAudioBlock _pipeline = {};

public:
  void ToHost(FBHostAudioBlock& host);
  void FromPlug(FBPlugAudioBlock const& plug);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugToHostProcessor);
};