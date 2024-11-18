#pragma once

struct FFProcessorMemory;

class FFOsciProcessor
{
  float _phase = 0.0f;

  void ProcessType(int moduleSlot, FFProcessorMemory& memory);
  template <class Calc>
  void ProcessType(int moduleSlot, FFProcessorMemory& memory, Calc calc);

public:
  void Process(int moduleSlot, FFProcessorMemory& memory);
};