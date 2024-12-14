#pragma once

#include <playground_plug/modules/master/FFMasterProcessor.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBVectorByteCount) FFMasterDSPState final
{
  friend class FFPlugProcessor;
  FFMasterProcessor processor = {};
public:
  FBFixedAudioBlock input = {};
  FBFixedAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterDSPState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend FBStaticModule FFMakeMasterTopo();
  std::array<TGlobalAcc, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterParamState final
{
  friend class FFMasterProcessor;
  friend FBStaticModule FFMakeMasterTopo();
  FFMasterAccParamState<TGlobalAcc> acc = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};