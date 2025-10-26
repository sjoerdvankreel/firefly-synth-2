#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeSettingsTopo(bool isFx)
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Settings";
  result->slotCount = 1;
  result->id = "{3689411E-F31C-4F8C-BE3D-6F87938A1A1B}";
  result->params.resize((int)FFSettingsParam::Count);
  auto selectModule = [](auto& state) { return &state.global.settings; };

  auto& hostSmoothTime = result->params[(int)FFSettingsParam::HostSmoothTime];
  hostSmoothTime.storeInPatch = false;
  hostSmoothTime.mode = FBParamMode::Block;
  hostSmoothTime.defaultText = "2";
  hostSmoothTime.name = "External MIDI/Automation Smoothing";
  hostSmoothTime.display = "Ext Smth";
  hostSmoothTime.slotCount = 1;
  hostSmoothTime.unit = "Ms";
  hostSmoothTime.id = "{C0FEF40D-573A-4F3C-A51C-5906EAE28694}";
  hostSmoothTime.type = FBParamType::Linear;
  hostSmoothTime.Linear().min = 0.0f;
  hostSmoothTime.Linear().max = 0.2f;
  hostSmoothTime.Linear().editSkewFactor = 0.5f;
  hostSmoothTime.Linear().displayMultiplier = 1000.0f;
  auto selectHostSmoothTime = [](auto& module) { return &module.block.hostSmoothTime; };
  hostSmoothTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectHostSmoothTime);

  auto& receiveNotes = result->params[(int)FFSettingsParam::ReceiveNotes];
  receiveNotes.storeInPatch = false;
  receiveNotes.mode = FBParamMode::Block;
  receiveNotes.name = "Receive MIDI Notes";
  receiveNotes.display = "Rcve Notes";
  receiveNotes.slotCount = 1;
  receiveNotes.defaultText = isFx ? "Off" : "On";
  receiveNotes.id = "{D996862F-74B0-460F-A50B-61261B400961}";
  receiveNotes.type = FBParamType::Boolean;
  auto selectReceiveNotes = [](auto& module) { return &module.block.receiveNotes; };
  receiveNotes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReceiveNotes);
  receiveNotes.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReceiveNotes);
  receiveNotes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReceiveNotes);

  return result;
}