#pragma once

inline float
FFGraphFilterFreqMultiplier(bool graph, float sampleRate, float maxFilterFreq)
{
  if (graph && sampleRate < (maxFilterFreq * 2.0f))
    return sampleRate / (maxFilterFreq * 2.0f);
  return 1.0f;
}