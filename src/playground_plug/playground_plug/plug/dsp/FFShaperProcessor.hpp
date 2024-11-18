#pragma once

struct FFProcessorMemory;

class FFShaperProcessor
{
public:
  void Process(int moduleSlot, FFProcessorMemory& memory);
};