#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <array>
#include <vector>

class FBVoiceManager;
struct FBFixedInputBlock;
struct FBPlugOutputBlock;

class FBSmoothingProcessor final
{
  FBVoiceManager* const _voiceManager;
  std::vector<int> _activeGlobalSmoothing = {};
  std::vector<int> _finishedGlobalSmoothing = {};
  std::vector<int> _activeGlobalSmoothingSamples = {};
  std::vector<int> _activeMIDISmoothing = {};
  std::vector<int> _finishedMIDISmoothing = {};
  std::array<int, FBMIDIEvent::MessageCount> _activeMIDISmoothingSamples = {};
  std::array<std::vector<int>, FBMaxVoices> _activeVoiceSmoothing = {};
  std::array<std::vector<int>, FBMaxVoices> _finishedVoiceSmoothing = {};
  std::array<std::vector<int>, FBMaxVoices> _activeVoiceSmoothingSamples = {};
  std::vector<FBAccAutoEvent> _accAutoBySampleThenParam = {};
  std::vector<FBMIDIEvent> _midiBySampleThenMessageThenCC = {};
  std::vector<FBAccModEvent> _accModBySampleThenParamThenNote = {};

  void RemoveIfNotExists(std::vector<int>& params, int param); 
  void InsertIfNotExists(std::vector<int>& params, int param);
  void InsertMustNotExist(std::vector<int>& params, int param); 

  void BeginMIDISmoothing(int eventId, int smoothingSamples);
  void BeginGlobalSmoothing(int param, int smoothingSamples);
  void BeginVoiceSmoothing(int voice, int param, int smoothingSamples);
  std::vector<int>::iterator FinishMIDISmoothing(std::vector<int>::iterator iter);
  std::vector<int>::iterator FinishGlobalSmoothing(std::vector<int>::iterator iter);
  std::vector<int>::iterator FinishVoiceSmoothing(int voice, std::vector<int>::iterator iter);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBSmoothingProcessor);
  FBSmoothingProcessor(FBVoiceManager* voiceManager, int paramCount);
  void ProcessSmoothing(FBFixedInputBlock const& input, FBPlugOutputBlock& output, int smoothingSamples);
}; 