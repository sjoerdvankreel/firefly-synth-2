#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <memory>

struct FBStaticModule;

template <class TBlock>
class alignas(alignof(TBlock)) FFPanicBlockParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakePanicTopo();
  std::array<TBlock, 1> flushAudioToggle = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPanicBlockParamState);
};

template <class TBlock>
class alignas(alignof(TBlock)) FFPanicParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakePanicTopo();
  FFPanicBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPanicParamState);
};