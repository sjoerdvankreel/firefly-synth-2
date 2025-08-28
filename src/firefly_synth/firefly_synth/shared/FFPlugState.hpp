#pragma once

#include <firefly_synth/dsp/plug/FFVoiceProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvState.hpp>
#include <firefly_synth/modules/lfo/FFLFOState.hpp>
#include <firefly_synth/modules/mix/FFVMixState.hpp>
#include <firefly_synth/modules/mix/FFGMixState.hpp>
#include <firefly_synth/modules/osci/FFOsciState.hpp>
#include <firefly_synth/modules/midi/FFMIDIState.hpp>
#include <firefly_synth/modules/echo/FFEchoState.hpp>
#include <firefly_synth/modules/effect/FFEffectState.hpp>
#include <firefly_synth/modules/master/FFMasterState.hpp>
#include <firefly_synth/modules/output/FFOutputState.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModState.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixState.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsState.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleState.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <firefly_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <firefly_base/base/state/proc/FBVoiceBlockParamState.hpp>
#include <firefly_base/base/state/proc/FBGlobalBlockParamState.hpp>
#include <firefly_base/base/state/exchange/FBModuleExchangeState.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <array>

struct FFGUIState final
{
  std::array<FFEchoGUIState, 1> vEcho = {};
  std::array<FFEchoGUIState, 1> gEcho = {};
  std::array<FFGUISettingsGUIState, 1> guiSettings = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUIState);
};

struct FFGlobalExchangeState final
{
  std::array<FFLFOExchangeState, FFLFOCount> gLFO = {};
  std::array<FBModuleProcSingleExchangeState, 1> gMix = {};
  std::array<FBModuleProcSingleExchangeState, 1> gEcho = {};
  std::array<FBModuleProcSingleExchangeState, 1> master = {};
  std::array<FBModuleProcSingleExchangeState, 1> output = {};
  std::array<FBModuleProcSingleExchangeState, 1> gMatrix = {};
  std::array<FBModuleProcSingleExchangeState, FFEffectCount> gEffect = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalExchangeState);
};

struct FFVoiceExchangeState final
{
  std::array<FFLFOExchangeState, FFLFOCount> vLFO = {};
  std::array<FBModuleProcSingleExchangeState, 1> vMix = {};
  std::array<FBModuleProcSingleExchangeState, 1> vEcho = {};
  std::array<FBModuleProcSingleExchangeState, 1> vMatrix = {};
  std::array<FBModuleProcSingleExchangeState, 1> osciMod = {};
  std::array<FBModuleProcSingleExchangeState, 1> voiceModule = {};
  std::array<FBModuleProcSingleExchangeState, FFEnvCount> env = {};
  std::array<FBModuleProcSingleExchangeState, FFOsciCount> osci = {};
  std::array<FBModuleProcSingleExchangeState, FFEffectCount> vEffect = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceExchangeState);
};

struct alignas(FBSIMDAlign) FFGlobalDSPState final
{
  FFMIDIDSPState midi = {};
  FFOutputDSPState output = {};
  FFMasterDSPState master = {};
  FFEchoDSPState<true> gEcho = {};
  FFModMatrixDSPState<true> gMatrix = {};
  std::array<FFLFODSPState, FFLFOCount> gLFO = {};
  std::array<FFEffectDSPState, FFEffectCount> gEffect = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalDSPState);
};

struct alignas(FBSIMDAlign) FFVoiceDSPState final
{
  FFOsciModDSPState osciMod = {};
  FFVoiceProcessor processor = {};
  FFEchoDSPState<false> vEcho = {};
  FFVoiceModuleDSPState voiceModule = {};
  FFModMatrixDSPState<false> vMatrix = {};
  std::array<FFEnvDSPState, FFEnvCount> env = {};
  std::array<FFLFODSPState, FFLFOCount> vLFO = {};
  std::array<FFOsciDSPState, FFOsciCount> osci = {};
  std::array<FFEffectDSPState, FFEffectCount> vEffect = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceDSPState);
};

struct alignas(FBSIMDAlign) FFProcDSPState final
{
  FFGlobalDSPState global = {};
  std::array<FFVoiceDSPState, FBMaxVoices> voice = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcDSPState);
}; 

template <class TBlock, class TAccurate>
struct alignas(alignof(TAccurate)) FFGlobalParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalParamState);
  std::array<FFGMixParamState<TAccurate>, 1> gMix = {};
  std::array<FFEchoParamState<TBlock, TAccurate>, 1> gEcho = {};
  std::array<FFGUISettingsParamState<TBlock>, 1> guiSettings = {};
  std::array<FFMasterParamState<TBlock, TAccurate>, 1> master = {};
  std::array<FFOutputParamState<TBlock, TAccurate>, 1> output = {};
  std::array<FFLFOParamState<TBlock, TAccurate>, FFLFOCount> gLFO = {};
  std::array<FFModMatrixParamState<TBlock, TAccurate, true>, 1> gMatrix = {};
  std::array<FFEffectParamState<TBlock, TAccurate>, FFEffectCount> gEffect = {};
};

template <class TBlock, class TAccurate>
struct alignas(alignof(TAccurate)) FFVoiceParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceParamState);
  std::array<FFVMixParamState<TAccurate>, 1> vMix = {};
  std::array<FFEchoParamState<TBlock, TAccurate>, 1> vEcho = {};
  std::array<FFOsciModParamState<TBlock, TAccurate>, 1> osciMod = {};
  std::array<FFEnvParamState<TBlock, TAccurate>, FFEnvCount> env = {};
  std::array<FFLFOParamState<TBlock, TAccurate>, FFLFOCount> vLFO = {};
  std::array<FFOsciParamState<TBlock, TAccurate>, FFOsciCount> osci = {};
  std::array<FFVoiceModuleParamState<TBlock, TAccurate>, 1> voiceModule = {};
  std::array<FFModMatrixParamState<TBlock, TAccurate, false>, 1> vMatrix = {};
  std::array<FFEffectParamState<TBlock, TAccurate>, FFEffectCount> vEffect = {};
};

struct FFScalarParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarParamState);
  FFVoiceParamState<double, double> voice = {};
  FFGlobalParamState<double, double> global = {};
};

struct FFScalarState final
{
  FFScalarParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

struct FFExchangeParamState final
{
  typedef std::array<float, FBMaxVoices> ScalarPerVoice;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFExchangeParamState);
  FFGlobalParamState<float, float> global = {};
  FFVoiceParamState<ScalarPerVoice, ScalarPerVoice> voice = {};
};

struct FFExchangeState final
{
  FBHostExchangeState host = {};
  FFExchangeParamState param = {};
  FFGlobalExchangeState global = {};
  std::array<FBVoiceInfo, FBMaxVoices> voices = {};
  std::array<FFVoiceExchangeState, FBMaxVoices> voice = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFExchangeState);
};

struct alignas(FBSIMDAlign) FFProcParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcParamState);
  FFVoiceParamState<FBVoiceBlockParamState, FBVoiceAccParamState> voice = {};
  FFGlobalParamState<FBGlobalBlockParamState, FBGlobalAccParamState> global = {};
};

struct alignas(FBSIMDAlign) FFProcState final
{
  FFProcDSPState dsp = {};
  FFProcParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
};