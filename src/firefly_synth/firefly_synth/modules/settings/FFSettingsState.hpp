#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <memory>

struct FBStaticModule;

template <class TBlock>
class alignas(alignof(TBlock)) FFSettingsBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceModuleProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool);
  std::array<TBlock, 1> tuning = {};
  std::array<TBlock, 1> tuneOnNote = {};
  std::array<TBlock, 1> tuneMasterPB = {};
  std::array<TBlock, 1> tuneVoiceCoarse = {};
  std::array<TBlock, 1> receiveNotes = {};
  std::array<TBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFSettingsBlockParamState);
};

template <class TBlock>
class alignas(alignof(TBlock)) FFSettingsParamState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceModuleProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool);
  FFSettingsBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFSettingsParamState);
};
