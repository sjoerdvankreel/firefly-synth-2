#pragma once

struct FBHostInputBlock;
class FBRawStereoBlockView;

class IFBHostBlockProcessor
{
public:
  virtual ~IFBHostBlockProcessor() {}
  virtual void ProcessHost(FBHostInputBlock const& input, FBRawStereoBlockView& output) = 0;
};