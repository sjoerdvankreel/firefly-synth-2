#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBProcState;
class FBFixedAudioBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  FB_NOCOPY_MOVE_DEFCTOR(IFBPlugProcessor);

  virtual FBProcState* State() = 0;
  virtual void ProcessPlug(FBFixedAudioBlock const& input, FBFixedAudioBlock& output) = 0;
};