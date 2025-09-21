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
  FFMasterTuningMode tuningMode = {};
  FFMasterPitchBendTarget bendTarget = {};
  FBSArray<float, FBFixedBlockSamples> bendAmountInSemis = {};

  FBSArray<float, FBFixedBlockSamples> outputMod = {};
  FBSArray<float, FBFixedBlockSamples> outputBend = {};
  FBSArray2<float, FBFixedBlockSamples, FFMasterAuxCount> outputAux = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFMasterBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TBlock, 1> tuningMode = {};
  std::array<TBlock, 1> hostSmoothTime = {};
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