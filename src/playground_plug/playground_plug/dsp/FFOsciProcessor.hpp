#if 0

#pragma once

struct FFProcessorMemory;
struct FBProcessorContext;

class FFOsciProcessor
{
  float _phase = 0.0f;

  void ProcessType(FBProcessorContext const& context, FFProcessorMemory& memory);
  template <class Calc>
  void ProcessType(FBProcessorContext const& context, FFProcessorMemory& memory, Calc calc);

public:
  void Process(FBProcessorContext const& context, FFProcessorMemory& memory);
};

#endif