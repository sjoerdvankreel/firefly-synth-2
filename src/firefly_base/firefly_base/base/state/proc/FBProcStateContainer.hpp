#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBMemoryPool.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/state/proc/FBProcParamState.hpp>
#include <firefly_base/dsp/shared/FBNoteMatrix.hpp>

#include <array>
#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBHostGUIContext;
class FBScalarStateContainer;
class FBExchangeStateContainer;

class FBProcStateContainer final
{
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBSmoothingProcessor;

  void* _rawState;
  void (*_freeRawState)(void*);
  FBSpecialParams _special;

  FBMemoryPool _memoryPool = {};
  int _smoothingDurationSamples = -1;
  std::vector<FBProcParamState> _params = {};

  // Very VST3-centric,
  // we treat all midi messages like continuous global params.
  std::array<FBGlobalAccParamState, FBMIDIEvent::MessageCount> _midiParams = {};
  std::array<FBGlobalAccParamState, (int)FBNoteMatrixEntry::Count> _noteMatrixSmth = {};

  void InitProcessing(int index, int voice, float value);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBProcStateContainer);
  FBProcStateContainer(FBRuntimeTopo const& topo);
  ~FBProcStateContainer() { _freeRawState(_rawState); }

  void SetSmoothingCoeffs(int sampleCount);
  void InitProcessing(int index, float value);
  void InitProcessing(FBHostGUIContext const* context);
  void InitProcessing(FBScalarStateContainer const& scalar);
  void InitProcessing(FBExchangeStateContainer const& exchange);

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  FBSpecialParams const& Special() const { return _special; }
  template <class T> T* RawAs() { return static_cast<T*>(Raw()); }
  template <class T> T const* RawAs() const { return static_cast<T const*>(Raw()); }

  FBMemoryPool* MemoryPool() { return &_memoryPool; }
  std::vector<FBProcParamState>& Params() { return _params; }
  std::vector<FBProcParamState> const& Params() const { return _params; }
  std::array<FBGlobalAccParamState, FBMIDIEvent::MessageCount>& MIDIParams() { return _midiParams; }
  std::array<FBGlobalAccParamState, FBMIDIEvent::MessageCount> const& MIDIParams() const { return _midiParams; }
  std::array<FBGlobalAccParamState, (int)FBNoteMatrixEntry::Count>& NoteMatrixSmth() { return _noteMatrixSmth; }
  std::array<FBGlobalAccParamState, (int)FBNoteMatrixEntry::Count> const& NoteMatrixSmth() const { return _noteMatrixSmth; }
};