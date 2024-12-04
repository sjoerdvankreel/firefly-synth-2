#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class FBFixedAudioBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  FB_NOCOPY_MOVE_DEFCTOR(IFBPlugProcessor);
  virtual void ProcessPlug(FBFixedAudioBlock const& input, FBFixedAudioBlock& output) = 0;
};