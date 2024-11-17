#pragma once

struct FFProcessorBlock;

class FFOsciProcessor
{
  float _phase = 0.0f;

public:
  void Process(int moduleSlot, FFProcessorBlock& block);
};