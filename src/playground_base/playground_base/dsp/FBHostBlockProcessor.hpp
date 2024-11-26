#pragma once

struct FBHostInputBlock;
class FBRawAudioBlockView;

class IFBHostBlockProcessor
{
public:
  virtual ~IFBHostBlockProcessor() {}
  virtual void ProcessHost(FBHostInputBlock const& input, FBRawAudioBlockView& output) = 0;
};