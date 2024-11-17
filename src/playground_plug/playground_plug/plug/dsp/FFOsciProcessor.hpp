#pragma once

struct FFPluginBlock;

class FFOsciProcessor
{
  float _phase = 0.0f;

public:
  void Process(int moduleSlot, FFPluginBlock& block);
};