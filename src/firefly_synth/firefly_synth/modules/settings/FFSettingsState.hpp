#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <memory>

struct FBStaticModule;

template <class TBlock>
class alignas(alignof(TBlock)) FFSettingsBlockParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool);
  std::array<TBlock, 1> receiveNotes = {};
  std::array<TBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFSettingsBlockParamState);
};

template <class TBlock>
class alignas(alignof(TBlock)) FFSettingsParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool);
  FFSettingsBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFSettingsParamState);
};

class FFSettingsGUIState final
{
  friend struct FFGUIState;
  friend std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool);

  std::array<double, 1> userScale = {};
  std::array<double, 1> hilightMod = {};
  std::array<double, 1> hilightTweak = {};
  std::array<double, 1> fxSelectedTab = {};
  std::array<double, 1> oscSelectedTab = {};
  std::array<double, 1> mixSelectedTab = {};
  std::array<double, 1> envSelectedTab = {};
  std::array<double, 1> lfoSelectedTab = {};
  std::array<double, 1> echoSelectedTab = {};
  std::array<double, 1> knobVisualsMode = {};
  std::array<double, 1> graphVisualsMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFSettingsGUIState);
};
