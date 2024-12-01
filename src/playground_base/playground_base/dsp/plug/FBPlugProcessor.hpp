#pragma once

class FBPlugAudioBlock;
struct FBPlugInputBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  virtual void ProcessPlug(
    FBPlugInputBlock const& input, FBPlugAudioBlock& output) = 0;
};