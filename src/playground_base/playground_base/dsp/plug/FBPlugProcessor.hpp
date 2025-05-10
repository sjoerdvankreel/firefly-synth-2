#pragma once

#include <playground_base/base/shared/FBUtility.hpp>

struct FBPlugInputBlock;
struct FBPlugOutputBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  FB_NOCOPY_MOVE_DEFCTOR(IFBPlugProcessor);

  virtual void LeaseVoices(FBPlugInputBlock const& input) = 0;
  virtual void ProcessPreVoice(FBPlugInputBlock const& input) = 0;
  virtual void ProcessVoice(FBPlugInputBlock const& input, int voice) = 0;
  virtual void ProcessPostVoice(FBPlugInputBlock const& input, FBPlugOutputBlock& output) = 0;
};