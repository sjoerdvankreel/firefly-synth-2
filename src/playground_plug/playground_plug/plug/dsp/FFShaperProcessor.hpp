#pragma once

struct FFProcessorBlock;

class FFShaperProcessor
{
public:
  void Process(int moduleSlot, FFProcessorBlock& block);
};