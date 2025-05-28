#pragma once

#include <playground_plug/dsp/FFVoiceProcessor.hpp>
#include <playground_plug/modules/env/FFEnvState.hpp>
#include <playground_plug/modules/glfo/FFGLFOState.hpp>
#include <playground_plug/modules/osci/FFOsciState.hpp>
#include <playground_plug/modules/effect/FFEffectState.hpp>
#include <playground_plug/modules/master/FFMasterState.hpp>
#include <playground_plug/modules/output/FFOutputState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterState.hpp>
#include <playground_plug/modules/physical/FFPhysState.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModState.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsState.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/proc/FBVoiceBlockParamState.hpp>
#include <playground_base/base/state/proc/FBGlobalBlockParamState.hpp>
#include <playground_base/base/state/exchange/FBModuleExchangeState.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

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
  std::array<FBModuleProcExchangeState, FFPhysCount> phys = {};
  std::array<FBModuleProcExchangeState, FFEffectCount> effect = {};
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
  std::array<FFPhysDSPState, FFPhysCount> phys = {};
  std::array<FFEffectDSPState, FFEffectCount> effect = {};
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
  std::array<FFPhysParamState<TVoiceBlock, TVoiceAcc>, FFPhysCount> phys = {};
  std::array<FFEffectParamState<TVoiceBlock, TVoiceAcc>, FFEffectCount> effect = {};
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