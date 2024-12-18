#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class FBHostProcessor;

class IFBHostProcessContext
{
public:
  virtual void ProcessVoices() = 0;
  FB_NOCOPY_MOVE_DEFCTOR(IFBHostProcessContext);
};