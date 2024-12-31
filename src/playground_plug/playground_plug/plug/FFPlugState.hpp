#pragma once

#include <playground_plug/modules/glfo/FFGLFOState.hpp>
#include <playground_plug/modules/glpf/FFGLPFState.hpp>
#include <playground_plug/modules/osci/FFOsciState.hpp>
#include <playground_plug/modules/master/FFMasterState.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBVoiceBlockParamState.hpp>
#include <playground_base/base/state/FBGlobalBlockParamState.hpp>

#include <array>

struct alignas(sizeof(FBFloatVector)) FFGlobalDSPState final
{
  FFMasterDSPState master = {};
  std::array<FFGLPFDSPState, FFGLPFCount> glpf = {};
  std::array<FFGLFODSPState, FFGLFOCount> glfo = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalDSPState);
};

struct alignas(sizeof(FBFloatVector)) FFVoiceDSPState final
{
  FBFixedAudioBlock output = {};
  std::array<FFOsciDSPState, FFOsciCount> osci = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceDSPState);
};

struct alignas(sizeof(FBFloatVector)) FFProcDSPState final
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
  std::array<FFGLPFParamState<TGlobalBlock, TGlobalAcc>, FFGLPFCount> glpf = {};
  std::array<FFGLFOParamState<TGlobalBlock, TGlobalAcc>, FFGLFOCount> glfo = {};
};

template <class TVoiceBlock, class TVoiceAcc>
struct alignas(alignof(TVoiceAcc)) FFVoiceParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceParamState);
  std::array<FFOsciParamState<TVoiceBlock, TVoiceAcc>, FFOsciCount> osci = {};
};

struct FFScalarParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarParamState);
  FFVoiceParamState<float, float> voice = {};
  FFGlobalParamState<float, float> global = {};
};

struct FFScalarState final
{
  FFScalarParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

struct alignas(sizeof(FBFloatVector)) FFProcParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcParamState);
  FFVoiceParamState<FBVoiceBlockParamState, FBVoiceAccParamState> voice = {};
  FFGlobalParamState<FBGlobalBlockParamState, FBGlobalAccParamState> global = {};
};

struct alignas(sizeof(FBFloatVector)) FFProcState final
{
  FFProcDSPState dsp = {};
  FFProcParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
};