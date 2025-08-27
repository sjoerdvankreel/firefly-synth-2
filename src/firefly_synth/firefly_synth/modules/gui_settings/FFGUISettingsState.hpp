#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <memory>

struct FBStaticModule;

class FFGUISettingsGUIState final
{
  friend struct FFGUIState;
  std::array<double, 1> userScale = {};
  std::array<double, 1> showMatrix = {};
  std::array<double, 1> fxSelectedTab = {};
  std::array<double, 1> oscSelectedTab = {};
  std::array<double, 1> mixSelectedTab = {};
  std::array<double, 1> envSelectedTab = {};
  std::array<double, 1> lfoSelectedTab = {};
  std::array<double, 1> echoSelectedTab = {};
  std::array<double, 1> knobRenderMode = {};
  std::array<double, 1> graphRenderMode = {};
  friend std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUISettingsGUIState);
};

template <class TBlock>
class alignas(alignof(TBlock)) FFGUISettingsBlockParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();
  std::array<TBlock, 1> flushDelayToggle = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUISettingsBlockParamState);
};

template <class TBlock>
class FFGUISettingsParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();
  FFGUISettingsBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUISettingsParamState);
};
