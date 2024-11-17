#pragma once

struct FFPluginBlock;

class FFOsciProcessor
{
  float _phase = 0.0f;

  void ProcessOff(int moduleSlot, FFPluginBlock& block);
  template <class PhaseToOut>
  void ProcessPhase(int moduleSlot, FFPluginBlock& block, PhaseToOut phaseToOut);

public:
  void Process(int moduleSlot, FFPluginBlock& block);
};