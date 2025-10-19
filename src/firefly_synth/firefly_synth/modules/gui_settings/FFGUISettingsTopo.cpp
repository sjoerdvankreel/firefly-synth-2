#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGUISettingsTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "UI";
  result->slotCount = 1;
  result->id = "{2407B76A-7FB3-4BD6-B6FD-B1F610AF8147}";
  result->guiParams.resize((int)FFGUISettingsGUIParam::Count);
  auto selectGuiModule = [](auto& state) { return &state.guiSettings; }; 

  auto& guiShowMatrix = result->guiParams[(int)FFGUISettingsGUIParam::ShowMatrix];
  guiShowMatrix.defaultText = "Off";
  guiShowMatrix.name = "Show Matrix";
  guiShowMatrix.slotCount = 1;
  guiShowMatrix.id = "{FEB66217-BE68-4B94-A8D3-009EE307BBB5}";
  guiShowMatrix.type = FBParamType::Boolean;
  auto selectGuiShowMatrix = [](auto& module) { return &module.showMatrix; };
  guiShowMatrix.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiShowMatrix);

  return result;
}