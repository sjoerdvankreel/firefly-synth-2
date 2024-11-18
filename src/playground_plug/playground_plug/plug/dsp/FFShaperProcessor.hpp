#pragma once

struct FFProcessorMemory;
struct FBProcessorContext;

class FFShaperProcessor
{
public:
  void Process(FBProcessorContext const& context, FFProcessorMemory& memory);
};