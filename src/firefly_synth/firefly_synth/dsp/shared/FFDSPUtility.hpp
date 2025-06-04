#pragma once

inline float
FFGraphFilterFreqMultiplier(bool graph, float sampleRate, float maxFilterFreq)
{
  if (graph && sampleRate < (maxFilterFreq * 2.0f))
    return sampleRate / (maxFilterFreq * 2.0f);
  return 1.0f;
}

inline float
WithKeyboardTracking(float freq, float key, float trackingKey, float trackingAmt, float minFreq, float maxFreq)
{
  float expo = (key - 60.0f + trackingKey) / 12.0f * trackingAmt;
  return std::clamp(freq * std::pow(2.0f, expo), minFreq, maxFreq);
}