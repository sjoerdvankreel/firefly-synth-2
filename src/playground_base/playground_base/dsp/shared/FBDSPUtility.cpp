#include <playground_base/dsp/shared/FBDSPUtility.hpp>

static std::array<float, FBNoteCentCount>
MakePitchToFreqTable();
static std::array<float, FBNoteCentCount>
pitchToFreqTable = MakePitchToFreqTable();

std::array<float, FBNoteCentCount> const&
FBPitchToFreqTable()
{
  return pitchToFreqTable;
}

static std::array<float, FBNoteCentCount>
MakePitchToFreqTable()
{
  std::array<float, FBNoteCentCount> result = {};
  for(int n = 0; n < FBNoteCount; n++)
    for (int c = 0; c < FBCentCount; c++)
      result[n * 100 + c] = FBPitchToFreqAccurateRaw(n + c / 100.0f);
  return result;
}
