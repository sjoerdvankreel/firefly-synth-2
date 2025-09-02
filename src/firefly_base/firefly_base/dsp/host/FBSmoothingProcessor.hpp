#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <array>
#include <vector>

class FBVoiceManager;
struct FBFixedInputBlock;
struct FBPlugOutputBlock;

struct FBSmoothingState final
{
  std::vector<int> active = {};
  std::vector<int> finished = {};
  std::vector<int> activeSamples = {};

  FBSmoothingState(int paramCount);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBSmoothingState);

  void Begin(int param, int smoothingSamples);
  std::vector<int>::iterator Finish(std::vector<int>::iterator iter);
};

class FBSmoothingProcessor final
{
  FBVoiceManager* const _voiceManager;
  FBSmoothingState _midi;
  FBSmoothingState _global;
  FBSmoothingState _noteMatrix;
  std::array<FBSmoothingState, FBMaxVoices> _voice;

  std::vector<FBAccAutoEvent> _accAutoBySampleThenParam = {};
  std::vector<FBMIDIEvent> _midiBySampleThenMessageThenCC = {};
  std::vector<FBAccModEvent> _accModBySampleThenParamThenNote = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBSmoothingProcessor);
  FBSmoothingProcessor(FBVoiceManager* voiceManager, int paramCount);
  void ProcessSmoothing(FBFixedInputBlock const& input, FBPlugOutputBlock& output, int smoothingSamples);
};