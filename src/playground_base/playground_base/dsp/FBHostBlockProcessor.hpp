#pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>

class IFBHostBlockProcessor
{
public:
  virtual ~IFBHostBlockProcessor() {}
  virtual void ProcessHost(FBHostInputBlock const& input, FBRawStereoBlockView& output) = 0;
};