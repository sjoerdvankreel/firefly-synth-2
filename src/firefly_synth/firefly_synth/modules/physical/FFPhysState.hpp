#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/physical/FFPhysTopo.hpp>
#include <firefly_synth/modules/physical/FFPhysProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFPhysDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend struct PhysGraphRenderData;
  std::unique_ptr<FFPhysProcessor> processor = {};
public:
  FFPhysDSPState();
  ~FFPhysDSPState();
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPhysDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFPhysBlockParamState final
{
  friend class FFPhysProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakePhysTopo();
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> seed = {};
  std::array<TVoiceBlock, 1> poles = {};
  std::array<TVoiceBlock, 1> uniCount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPhysBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFPhysAccParamState final
{
  friend class FFPhysProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakePhysTopo();
  std::array<TVoiceAcc, 1> coarse = {};
  std::array<TVoiceAcc, 1> fine = {};
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> uniBlend = {};
  std::array<TVoiceAcc, 1> uniDetune = {};
  std::array<TVoiceAcc, 1> uniSpread = {};
  std::array<TVoiceAcc, 1> x = {};
  std::array<TVoiceAcc, 1> y = {};
  std::array<TVoiceAcc, 1> color = {};
  std::array<TVoiceAcc, 1> excite = {};
  std::array<TVoiceAcc, 1> lp = {};
  std::array<TVoiceAcc, 1> hp = {};
  std::array<TVoiceAcc, 1> range = {};
  std::array<TVoiceAcc, 1> center = {};
  std::array<TVoiceAcc, 1> damp = {};
  std::array<TVoiceAcc, 1> dampScale = {};
  std::array<TVoiceAcc, 1> feedback = {};
  std::array<TVoiceAcc, 1> feedbackScale = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPhysAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFPhysParamState final
{
  friend class FFPhysProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakePhysTopo();
  FFPhysAccParamState<TVoiceAcc> acc = {};
  FFPhysBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPhysParamState);
};