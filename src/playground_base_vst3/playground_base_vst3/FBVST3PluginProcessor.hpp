#pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>

class IFBVST3PluginProcessor
{
public:
  virtual ~IFBVST3PluginProcessor() {}
  virtual void ProcessHost(FBHostInputBlock const& input, FBRawStereoBlockView& output) = 0;
};