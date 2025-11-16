#pragma once

#include <libMTSClient.h>
#include <algorithm>
#include <cmath>

inline float
FBTuneReal(MTSClient* mtsClient, float pitchToTune, int channel)
{
  char pitchToTune0 = (char)std::clamp(std::floor(pitchToTune), 0.0f, 127.0f);
  char pitchToTune1 = (char)std::clamp(pitchToTune0 + 1, 0, 127);
  float pitchTuned0 = pitchToTune + (float)MTS_RetuningInSemitones(mtsClient, pitchToTune0, (char)channel);
  float pitchTuned1 = pitchToTune + (float)MTS_RetuningInSemitones(mtsClient, pitchToTune1, (char)channel);
  float lerp = pitchToTune - pitchToTune0;
  return (1.0f - lerp) * pitchTuned0 + lerp * pitchTuned1;
}