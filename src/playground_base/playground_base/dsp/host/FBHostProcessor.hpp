#pragma once

class FBHostAudioBlock;
struct FBHostInputBlock;

class IFBHostProcessor
{
public:
  virtual ~IFBHostProcessor() {}
  virtual void ProcessHost(FBHostInputBlock const& input, FBHostAudioBlock& output) = 0;
};