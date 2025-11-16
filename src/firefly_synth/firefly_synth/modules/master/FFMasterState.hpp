#pragma once

#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_synth/modules/master/FFMasterProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

class MTSClient;
struct FBStaticModule;

class alignas(FBSIMDAlign) FFMasterDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFMasterProcessor> processor = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFMasterDSPState);
  FFMasterDSPState() : processor(std::make_unique<FFMasterProcessor>()) {}

  bool mtsEspOn = {};
  MTSClient* mtsClient = {};
  FFMasterPitchBendTarget bendTarget = {};
  FBSArray<float, FBFixedBlockSamples> bendAmountInSemis = {};

  FBSArray<float, FBFixedBlockSamples> outputPB = {};
  FBSArray<float, FBFixedBlockSamples> outputMod = {};
  FBSArray<float, FBFixedBlockSamples> outputPBRaw = {};
  FBSArray2<float, FBFixedBlockSamples, FFMasterAuxCount> outputAux = {};

  // These are only microtuned if user selects "tune master matrix".
  // Just a perf optimization, tuning 10 sources per-sample is expensive.
  // And i wouldnt expect a microtuning user to always go for global-filters-in-tune-by-means-of-the-mod-matrix.
  // But, it can be done. Just costs a bit of cycles.
  FBSArray<float, FBFixedBlockSamples> outputLowKeyPitch = {};
  FBSArray<float, FBFixedBlockSamples> outputHighKeyPitch = {};
  FBSArray<float, FBFixedBlockSamples> outputLastKeyPitch = {};
  FBSArray<float, FBFixedBlockSamples> outputLowVeloPitch = {};
  FBSArray<float, FBFixedBlockSamples> outputHighVeloPitch = {};
  FBSArray<float, FBFixedBlockSamples> outputLowKeyPitchSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputHighKeyPitchSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputLastKeyPitchSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputLowVeloPitchSmth = {};
  FBSArray<float, FBFixedBlockSamples> outputHighVeloPitchSmth = {};

  // This one is tuned if tuning option is on, regardles of "tune master matrix", 
  // as we need it for global-filters-pitchtracking-mode (without the matrix).
  FBSArray<float, FBFixedBlockSamples> outputLastKeyPitchTunedRaw = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFMasterBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TBlock, 1> pitchBendRange = {};
  std::array<TBlock, 1> pitchBendTarget = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TAccurate, 1> modWheel = {};
  std::array<TAccurate, 1> pitchBend = {};
  std::array<TAccurate, FFMasterAuxCount> aux = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFMasterParamState final
{
  friend class FFPlugProcessor;
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterAccParamState<TAccurate> acc = {};
  FFMasterBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};