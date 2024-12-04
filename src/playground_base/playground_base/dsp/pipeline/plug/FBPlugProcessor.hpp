#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBPlugInputBlock;
class FBFixedAudioBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  FB_NOCOPY_MOVE_DEFCTOR(IFBPlugProcessor);
  virtual void ProcessPlug(FBPlugInputBlock const& input, FBFixedAudioBlock& output) = 0;
};