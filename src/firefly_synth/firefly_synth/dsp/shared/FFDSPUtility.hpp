#pragma once

inline float
MultiplyClamp(
  float val, float mul, float min, float max)
{
  return std::clamp(val * mul, min, max);
}

inline float
FFGraphFilterFreqMultiplier(
  bool graph, float sampleRate, float maxFilterFreq)
{
  if (graph && sampleRate < (maxFilterFreq * 2.0f))
    return sampleRate / (maxFilterFreq * 2.0f);
  return 1.0f;
}

inline float 
KeyboardTrackingMultiplier(
  float key, float trackingKey, float trackingAmt)
{
  if (trackingAmt == 0.0f)
    return 1.0f;
  return std::pow(2.0f, (key - 60.0f + trackingKey) / 12.0f * trackingAmt);
}