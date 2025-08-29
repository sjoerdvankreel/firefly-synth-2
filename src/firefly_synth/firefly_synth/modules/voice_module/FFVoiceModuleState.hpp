#pragma once

#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFVoiceModuleDSPState final
{
  friend class FFVoiceModuleProcessor;
  std::unique_ptr<FFVoiceModuleProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFVoiceModuleDSPState);
  FFVoiceModuleDSPState() : processor(std::make_unique<FFVoiceModuleProcessor>()) {}
};

template <class TBlock>
class alignas(alignof(TBlock)) FFVoiceModuleBlockParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeVoiceModuleTopo();
  std::array<TBlock, 1> portaType = {};
  std::array<TBlock, 1> portaMode = {};
  std::array<TBlock, 1> portaSync = {};
  std::array<TBlock, 1> portaTime = {};
  std::array<TBlock, 1> portaBars = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceModuleBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFVoiceModuleAccParamState final
{
  friend class FFVoiceModuleProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVoiceModuleTopo();
  std::array<TAccurate, 1> fine = {};
  std::array<TAccurate, 1> coarse = {};
  std::array<TAccurate, 1> lfo1ToFine = {};
  std::array<TAccurate, 1> env1ToCoarse = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceModuleAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFVoiceModuleParamState final
{
  friend class FFVoiceModuleProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVoiceModuleTopo();
  FFVoiceModuleAccParamState<TAccurate> acc = {};
  FFVoiceModuleBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceModuleParamState);
};