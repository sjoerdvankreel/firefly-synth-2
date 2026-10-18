#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBMemoryPool.hpp>
#include <firefly_base/base/state/proc/FBProcParamState.hpp>
#include <firefly_base/dsp/shared/FBNoteMatrix.hpp>

#include <array>
#include <vector>

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

  FBMemoryPool _memoryPool = {};
  int _smoothingDurationSamples = -1;
  std::vector<FBProcParamState> _params = {};

  // Very VST3-centric,
  // we treat all midi messages like continuous global params.
  FBNoteMatrix<FBGlobalAccParamState> _noteMatrixSmth = {};
  std::array<FBGlobalAccParamState, FBMIDIEvent::MessageCount> _midiParams = {};

  // Also very VST3-centric.
  // We keep a copy of an audio clip on both the processor side and the controller side.
  // Controller side is a very slimmed down version (1000 samples or so -- small).
  // It's either that or i ditch kDistributable and go with SingleComponentEffect
  // like everyone else does it. But, bit late to do that now.
  // Non-owning pointer! On the GUI side it comes from HostGUIContext.
  // On the DSP side it comes from the HostDSPContext.
  std::map<std::string, FBAudioClip> const* const _audioClips;

  void InitProcessing(int index, int voice, float value);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBProcStateContainer);
  FBProcStateContainer(FBRuntimeTopo const& topo, std::map<std::string, FBAudioClip> const* audioClips);
  ~FBProcStateContainer() { _freeRawState(_rawState); }

  void SetSmoothingCoeffs(int sampleCount);
  void InitProcessing(int index, float value);
  void InitProcessing(FBHostGUIContext const* context);
  void InitProcessing(FBScalarStateContainer const& scalar);
  void InitProcessing(FBExchangeStateContainer const& exchange);

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  template <class T> T* RawAs() { return static_cast<T*>(Raw()); }
  template <class T> T const* RawAs() const { return static_cast<T const*>(Raw()); }

  FBMemoryPool* MemoryPool() { return &_memoryPool; }
  std::vector<FBProcParamState>& Params() { return _params; }
  std::vector<FBProcParamState> const& Params() const { return _params; }
  std::map<std::string, FBAudioClip> const& AudioClips() const { return *_audioClips; }
  FBNoteMatrix<FBGlobalAccParamState>& NoteMatrixSmth() { return _noteMatrixSmth; }
  FBNoteMatrix<FBGlobalAccParamState> const& NoteMatrixSmth() const { return _noteMatrixSmth; }
  std::array<FBGlobalAccParamState, FBMIDIEvent::MessageCount>& MIDIParams() { return _midiParams; }
  std::array<FBGlobalAccParamState, FBMIDIEvent::MessageCount> const& MIDIParams() const { return _midiParams; }
};