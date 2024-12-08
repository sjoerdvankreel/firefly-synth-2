#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFGLFOProcessor.hpp>
#include <playground_plug/dsp/FFShaperProcessor.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBVoiceBlockParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <array>

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFGLFOBlockParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TGlobalBlock, 1> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOBlockParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOAccParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TGlobalAcc, 1> rate = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFGLFOAccParamState<TGlobalAcc> acc = {};
  FFGLFOBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOParamState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> type = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceAcc, 1> pitch = {};
  std::array<TVoiceAcc, FF_OSCI_GAIN_COUNT> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFOsciAccParamState<TVoiceAcc> acc = {};
  FFOsciBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFShaperBlockParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> clip = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFShaperAccParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceAcc, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFShaperParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFShaperAccParamState<TVoiceAcc> acc = {};
  FFShaperBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
struct alignas(alignof(TGlobalAcc)) FFGlobalParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalParamState);
  std::array<FFGLFOParamState<TGlobalBlock, TGlobalAcc>, FF_GLFO_COUNT> glfo = {};
};

template <class TVoiceBlock, class TVoiceAcc>
struct alignas(alignof(TVoiceAcc)) FFVoiceParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceParamState);
  std::array<FFOsciParamState<TVoiceBlock, TVoiceAcc>, FF_OSCI_COUNT> osci = {};
  std::array<FFShaperParamState<TVoiceBlock, TVoiceAcc>, FF_SHAPER_COUNT> shaper = {};
};

struct FFScalarState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
  FFVoiceParamState<float, float> voice = {};
  FFGlobalParamState<float, float> global = {};
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFProcParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcParamState);
  FFGlobalParamState<float, FBGlobalAccParamState> global = {};
  FFVoiceParamState<FBVoiceBlockParamState, FBVoiceAccParamState> voice = {};
};

class alignas(FB_FIXED_BLOCK_ALIGN) FFGLFODSPState final
{
  friend class FFGLFOProcessor;
  FBFixedCVBlock output = {};
public:
  FFGLFOProcessor processor = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFODSPState);
};

class alignas(FB_FIXED_BLOCK_ALIGN) FFOsciDSPState final
{
  friend class FFOsciProcessor;
  friend class FFPlugProcessor; // TODO
  FBFixedAudioBlock output = {};
public:
  FFOsciProcessor processor = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciDSPState);
};

class alignas(FB_FIXED_BLOCK_ALIGN) FFShaperDSPState final
{
  friend class FFShaperProcessor;
  FBFixedAudioBlock output = {};
public:
  FFShaperProcessor processor = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperDSPState);
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFGlobalDSPState final
{
  std::array<FFGLFODSPState, FF_GLFO_COUNT> glfo;
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFVoiceDSPState final
{
  FBFixedAudioBlock output = {};
  std::array<FFOsciDSPState, FF_OSCI_COUNT> osci = {};
  std::array<FFShaperDSPState, FF_SHAPER_COUNT> shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceDSPState);
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFProcDSPState final
{
  FFGlobalDSPState global = {};
  std::array<FFVoiceDSPState, FB_MAX_VOICES> voice = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcDSPState);
};

struct alignas(FB_FIXED_BLOCK_ALIGN) FFProcState final
{
  FFProcDSPState dsp = {};
  FFProcParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
};