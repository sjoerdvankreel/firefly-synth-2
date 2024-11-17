#pragma once

struct FFPluginBlock;

class FFShaperProcessor
{
public:
  void Process(int moduleSlot, FFPluginBlock& block);
};