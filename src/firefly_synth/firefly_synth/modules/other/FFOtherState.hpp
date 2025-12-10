#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <memory>

struct FBStaticModule;

template <class TBlock>
class alignas(alignof(TBlock)) FFOtherBlockParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOtherTopo();
  std::array<TBlock, 1> flushAudioToggle = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOtherBlockParamState);
};

template <class TBlock>
class alignas(alignof(TBlock)) FFOtherParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOtherTopo();
  FFOtherBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOtherParamState);
};