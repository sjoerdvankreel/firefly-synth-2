#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;
class FFMasterProcessor;

class alignas(FBSIMDAlign) FFMasterDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFMasterProcessor> processor = {};
public:
  FFMasterDSPState();
  ~FFMasterDSPState();
  FBFixedFloatAudioArray input = {};
  FBFixedFloatAudioArray output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFMasterDSPState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TGlobalAcc, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFMasterBlockParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TGlobalBlock, 1> voices = {};
  std::array<TGlobalBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterAccParamState<TGlobalAcc> acc = {};
  FFMasterBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};