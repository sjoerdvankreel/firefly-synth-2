#pragma once

class FBFixedAudioBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  virtual void ProcessPlug(
    FBFixedAudioBlock const& input, FBFixedAudioBlock& output) = 0;
};