#pragma once

struct FFPluginBlock;

class FFOsciProcessor
{
  float _phase = 0.0f;

  void ProcessType(int moduleSlot, FFPluginBlock& block);
  template <class Calc>
  void ProcessType(int moduleSlot, FFPluginBlock& block, Calc calc);

public:
  void Process(int moduleSlot, FFPluginBlock& block);
};