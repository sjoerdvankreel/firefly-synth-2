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
  hostSmoothTime.name = "MIDI/Auto Smth";
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
  receiveNotes.name = "Receive Notes";
  receiveNotes.slotCount = 1;
  receiveNotes.defaultText = isFx ? "Off" : "On";
  receiveNotes.id = "{D996862F-74B0-460F-A50B-61261B400961}";
  receiveNotes.type = FBParamType::Boolean;
  auto selectReceiveNotes = [](auto& module) { return &module.block.receiveNotes; };
  receiveNotes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReceiveNotes);
  receiveNotes.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReceiveNotes);
  receiveNotes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReceiveNotes);

  auto& tuning = result->params[(int)FFSettingsParam::Tuning];
  tuning.storeInPatch = false;
  tuning.mode = FBParamMode::Block;
  tuning.name = "Tuning";
  tuning.slotCount = 1;
  tuning.defaultText = "Off";
  tuning.id = "{7192D8E2-235B-46FA-93B6-BBE75F1088FD}";
  tuning.type = FBParamType::Boolean;
  auto selectTuning = [](auto& module) { return &module.block.tuning; };
  tuning.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuning);
  tuning.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuning);
  tuning.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuning);

  auto& tuneOnNote = result->params[(int)FFSettingsParam::TuneOnNote];
  tuneOnNote.storeInPatch = false;
  tuneOnNote.mode = FBParamMode::Block;
  tuneOnNote.name = "Tune On Note";
  tuneOnNote.display = "On Note";
  tuneOnNote.slotCount = 1;
  tuneOnNote.defaultText = "Off";
  tuneOnNote.id = "{12E81BF9-8123-4B7C-B1F6-976D0D7FE20A}";
  tuneOnNote.type = FBParamType::Boolean;
  auto selectTuneOnNote = [](auto& module) { return &module.block.tuneOnNote; };
  tuneOnNote.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuneOnNote);
  tuneOnNote.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuneOnNote);
  tuneOnNote.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuneOnNote);
  tuneOnNote.dependencies.enabled.audio.WhenSimple({ (int)FFSettingsParam::Tuning }, [](auto const& vs) { return vs[0] != 0; });

  auto& tuneMasterPB = result->params[(int)FFSettingsParam::TuneMasterPB];
  tuneMasterPB.storeInPatch = false;
  tuneMasterPB.mode = FBParamMode::Block;
  tuneMasterPB.name = "Tune Master PB";
  tuneMasterPB.display = "Master PB";
  tuneMasterPB.slotCount = 1;
  tuneMasterPB.defaultText = "Off";
  tuneMasterPB.id = "{EBBE4E94-C14C-4CE1-B70B-BE26E17EA155}";
  tuneMasterPB.type = FBParamType::Boolean;
  auto selectTuneMasterPB = [](auto& module) { return &module.block.tuneMasterPB; };
  tuneMasterPB.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuneMasterPB);
  tuneMasterPB.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuneMasterPB);
  tuneMasterPB.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuneMasterPB);
  tuneMasterPB.dependencies.enabled.audio.WhenSimple({ (int)FFSettingsParam::Tuning }, [](auto const& vs) { return vs[0] != 0; });

  auto& tuneMasterMatrix = result->params[(int)FFSettingsParam::TuneMasterMatrix];
  tuneMasterMatrix.storeInPatch = false;
  tuneMasterMatrix.mode = FBParamMode::Block;
  tuneMasterMatrix.name = "Tune Master Matrix";
  tuneMasterMatrix.display = "Master Matrix";
  tuneMasterMatrix.slotCount = 1;
  tuneMasterMatrix.defaultText = "Off";
  tuneMasterMatrix.id = "{8D196ECE-BF26-4735-804D-28641DEF06CA}";
  tuneMasterMatrix.type = FBParamType::Boolean;
  auto selectTuneMasterMatrix = [](auto& module) { return &module.block.tuneMasterMatrix; };
  tuneMasterMatrix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuneMasterMatrix);
  tuneMasterMatrix.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuneMasterMatrix);
  tuneMasterPB.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuneMasterMatrix);
  tuneMasterMatrix.dependencies.enabled.audio.WhenSimple({ (int)FFSettingsParam::Tuning }, [](auto const& vs) { return vs[0] != 0; });

  auto& tuneVoiceCoarse = result->params[(int)FFSettingsParam::TuneVoiceCoarse];
  tuneVoiceCoarse.storeInPatch = false;
  tuneVoiceCoarse.mode = FBParamMode::Block;
  tuneVoiceCoarse.name = "Tune Voice Coarse";
  tuneVoiceCoarse.display = "Voice Coarse";
  tuneVoiceCoarse.slotCount = 1;
  tuneVoiceCoarse.defaultText = "Off";
  tuneVoiceCoarse.id = "{DA9B29F1-E23F-4948-8608-9A323A93927F}";
  tuneVoiceCoarse.type = FBParamType::Boolean;
  auto selectTuneVoiceCoarse = [](auto& module) { return &module.block.tuneVoiceCoarse; };
  tuneVoiceCoarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuneVoiceCoarse);
  tuneVoiceCoarse.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuneVoiceCoarse);
  tuneVoiceCoarse.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuneVoiceCoarse);
  tuneVoiceCoarse.dependencies.enabled.audio.WhenSimple({ (int)FFSettingsParam::Tuning }, [](auto const& vs) { return vs[0] != 0; });

  auto& tuneVoiceMatrix = result->params[(int)FFSettingsParam::TuneVoiceMatrix];
  tuneVoiceMatrix.storeInPatch = false;
  tuneVoiceMatrix.mode = FBParamMode::Block;
  tuneVoiceMatrix.name = "Tune Voice Matrix";
  tuneVoiceMatrix.display = "Voice Matrix";
  tuneVoiceMatrix.slotCount = 1;
  tuneVoiceMatrix.defaultText = "Off";
  tuneVoiceMatrix.id = "{A65377D6-6D56-459E-A14B-B55C1565BC2D}";
  tuneVoiceMatrix.type = FBParamType::Boolean;
  auto selectTuneVoiceMatrix = [](auto& module) { return &module.block.tuneVoiceMatrix; };
  tuneVoiceMatrix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTuneVoiceMatrix);
  tuneVoiceMatrix.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTuneVoiceMatrix);
  tuneVoiceMatrix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTuneVoiceMatrix);
  tuneVoiceMatrix.dependencies.enabled.audio.WhenSimple({ (int)FFSettingsParam::Tuning }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}