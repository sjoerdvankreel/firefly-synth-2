#pragma once

#include <firefly_synth/dsp/plug/FFVoiceProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvState.hpp>
#include <firefly_synth/modules/glfo/FFGLFOState.hpp>
#include <firefly_synth/modules/osci/FFOsciState.hpp>
#include <firefly_synth/modules/effect/FFEffectState.hpp>
#include <firefly_synth/modules/master/FFMasterState.hpp>
#include <firefly_synth/modules/output/FFOutputState.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterState.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModState.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciState.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsState.hpp>

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
  std::array<FFGUISettingsGUIState, 1> guiSettings = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUIState);
};

struct FFGlobalExchangeState final
{
  std::array<FBModuleProcExchangeState, 1> master = {};
  std::array<FBModuleProcExchangeState, 1> output = {};
  std::array<FBModuleProcExchangeState, FFGLFOCount> gLFO = {};
  std::array<FBModuleProcExchangeState, FFGFilterCount> gFilter = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalExchangeState);
};

struct FFVoiceExchangeState final
{
  std::array<FBModuleProcExchangeState, 1> osciMod = {};
  std::array<FBModuleProcExchangeState, FFEnvCount> env = {};
  std::array<FBModuleProcExchangeState, FFOsciCount> osci = {};
  std::array<FBModuleProcExchangeState, FFEffectCount> effect = {};
  std::array<FBModuleProcExchangeState, FFStringOsciCount> stringOsci = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceExchangeState);
};

struct alignas(FBSIMDAlign) FFGlobalDSPState final
{
  FFMasterDSPState master = {};
  FFOutputDSPState output = {};
  std::array<FFGLFODSPState, FFGLFOCount> gLFO = {};
  std::array<FFGFilterDSPState, FFGFilterCount> gFilter = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalDSPState);
};

struct alignas(FBSIMDAlign) FFVoiceDSPState final
{
  FFOsciModDSPState osciMod = {};
  FFVoiceProcessor processor = {};
  std::array<FFEnvDSPState, FFEnvCount> env = {};
  std::array<FFOsciDSPState, FFOsciCount> osci = {};
  std::array<FFEffectDSPState, FFEffectCount> effect = {};
  std::array<FFStringOsciDSPState, FFStringOsciCount> stringOsci = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceDSPState);
};

struct alignas(FBSIMDAlign) FFProcDSPState final
{
  FFGlobalDSPState global = {};
  std::array<FFVoiceDSPState, FBMaxVoices> voice = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcDSPState);
}; 

template <class TGlobalBlock, class TGlobalAcc>
struct alignas(alignof(TGlobalAcc)) FFGlobalParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalParamState);
  std::array<FFMasterParamState<TGlobalBlock, TGlobalAcc>, 1> master = {};
  std::array<FFOutputParamState<TGlobalBlock, TGlobalAcc>, 1> output = {};
  std::array<FFGLFOParamState<TGlobalBlock, TGlobalAcc>, FFGLFOCount> gLFO = {};
  std::array<FFGFilterParamState<TGlobalBlock, TGlobalAcc>, FFGFilterCount> gFilter = {};
};

template <class TVoiceBlock, class TVoiceAcc>
struct alignas(alignof(TVoiceAcc)) FFVoiceParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceParamState);
  std::array<FFOsciModParamState<TVoiceBlock, TVoiceAcc>, 1> osciMod = {};
  std::array<FFEnvParamState<TVoiceBlock, TVoiceAcc>, FFEnvCount> env = {};
  std::array<FFOsciParamState<TVoiceBlock, TVoiceAcc>, FFOsciCount> osci = {};
  std::array<FFEffectParamState<TVoiceBlock, TVoiceAcc>, FFEffectCount> effect = {};
  std::array<FFStringOsciParamState<TVoiceBlock, TVoiceAcc>, FFStringOsciCount> stringOsci = {};
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