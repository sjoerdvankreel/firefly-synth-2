#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>

#include <playground_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeEnvBarsItems()
{
  return FBMakeBarsItems(true, { 1, 128 }, { 4, 1 });
}

std::unique_ptr<FBStaticModule>
FFMakeEnvTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Env";
  result->slotCount = FFEnvCount;
  result->graphRenderer = FFEnvRenderGraph;
  result->id = "{FC1DC75A-200C-4465-8CBE-0100E2C8FAF2}";
  result->params.resize((int)FFEnvParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.env; });
  auto selectGuiModule = [](auto& state) { return &state.env; };
  auto selectModule = [](auto& state) { return &state.voice.env; };

  auto& on = result->params[(int)FFEnvParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{40F1DCF0-03B1-4ABE-B6B5-A29BF2C8C229}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOn);
  on.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOn);
  on.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& sync = result->params[(int)FFEnvParam::Sync];
  sync.acc = false;
  sync.name = "Sync";
  sync.slotCount = 1;
  sync.id = "{B1128167-2EB6-4AFA-81B7-C4B7606502BB}";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSync);
  sync.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSync);
  sync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSync);
  sync.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& exp = result->params[(int)FFEnvParam::Exp];
  exp.acc = false;
  exp.defaultText = "Off";
  exp.name = "Exponential";
  exp.display = "Exp";
  exp.slotCount = 1;
  exp.id = "{1FF0F02E-E3D6-4DCB-8968-07CAC3DDC179}";
  exp.type = FBParamType::Boolean;
  auto selectExp = [](auto& module) { return &module.block.exp; };
  exp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectExp);
  exp.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectExp);
  exp.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectExp);
  exp.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& release = result->params[(int)FFEnvParam::Release];
  release.acc = false;
  release.defaultText = "Off";
  release.name = "Release";
  release.display = "Rls";
  release.slotCount = 1;
  release.id = "{38670133-4372-461F-ACB8-0E1E156BD3DF}";
  release.type = FBParamType::Discrete;
  release.Discrete().valueCount = FFEnvStageCount + 2;
  release.Discrete().valueFormatter = [](int v) { return v == 0 ? "Off" : std::to_string(v); };
  auto selectRelease = [](auto& module) { return &module.block.release; };
  release.scalarAddr = FFSelectScalarParamAddr(selectModule, selectRelease);
  release.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectRelease);
  release.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectRelease);
  release.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& sustain = result->params[(int)FFEnvParam::Sustain];
  sustain.acc = false;
  sustain.name = "Sustain";
  sustain.display = "Stn";
  sustain.slotCount = 1;
  sustain.id = "{FE555166-638C-4A3B-9DB9-9EBFE3762B23}";
  sustain.type = FBParamType::Boolean;
  auto selectSustain = [](auto& module) { return &module.block.sustain; };
  sustain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSustain);
  sustain.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSustain);
  sustain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSustain);
  sustain.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& loopStart = result->params[(int)FFEnvParam::LoopStart];
  loopStart.acc = false;
  loopStart.defaultText = "Off";
  loopStart.name = "Loop Start";
  loopStart.display = "Loop";
  loopStart.slotCount = 1;
  loopStart.id = "{97402218-4546-447E-A925-AB3DFD21A9F8}";
  loopStart.type = FBParamType::Discrete;
  loopStart.Discrete().valueCount = FFEnvStageCount + 2;
  loopStart.Discrete().valueFormatter = [](int v) { return v == 0 ? "Off" : std::to_string(v - 1); };
  auto selectLoopStart = [](auto& module) { return &module.block.loopStart; };
  loopStart.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLoopStart);
  loopStart.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectLoopStart);
  loopStart.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLoopStart);
  loopStart.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& loopLength = result->params[(int)FFEnvParam::LoopLength];
  loopLength.acc = false;
  loopLength.defaultText = "0";
  loopLength.name = "Loop Length";
  loopLength.display = "Length";
  loopLength.slotCount = 1;
  loopLength.id = "{078E33B3-02EB-4637-84E0-949E2830A0DB}";
  loopLength.type = FBParamType::Discrete;
  loopLength.Discrete().valueCount = FFEnvStageCount + 1;
  auto selectLoopLength = [](auto& module) { return &module.block.loopLength; };
  loopLength.scalarAddr = FFSelectScalarParamAddr(selectModule, selectLoopLength);
  loopLength.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectLoopLength);
  loopLength.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectLoopLength);
  loopLength.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::LoopStart }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& smoothTime = result->params[(int)FFEnvParam::SmoothTime];
  smoothTime.acc = false;
  smoothTime.defaultText = "0";
  smoothTime.display = "Smth";
  smoothTime.name = "Smooth Time";
  smoothTime.slotCount = 1;
  smoothTime.unit = "Sec";
  smoothTime.id = "{1F5A4312-4483-4F30-9E63-D0D81CF4CD0B}";
  smoothTime.type = FBParamType::Linear;
  smoothTime.Linear().min = 0.0f;
  smoothTime.Linear().max = 10.0f;
  smoothTime.Linear().editSkewFactor = 0.5f;
  auto selectSmoothTime = [](auto& module) { return &module.block.smoothTime; };
  smoothTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSmoothTime);
  smoothTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSmoothTime);
  smoothTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSmoothTime);
  smoothTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  smoothTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& smoothBars = result->params[(int)FFEnvParam::SmoothBars];
  smoothBars.acc = false;
  smoothBars.defaultText = "Off";
  smoothBars.display = "Smth";
  smoothBars.name = "Smooth Bars";
  smoothBars.slotCount = 1;
  smoothBars.unit = "Bars";
  smoothBars.id = "{EA0FC641-ED92-417A-ABCF-305A854F59C8}";
  smoothBars.type = FBParamType::Bars;
  smoothBars.Bars().items = MakeEnvBarsItems();
  auto selectSmoothBars = [](auto& module) { return &module.block.smoothBars; };
  smoothBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSmoothBars);
  smoothBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSmoothBars);
  smoothBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSmoothBars);
  smoothBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  smoothBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& stageLevel = result->params[(int)FFEnvParam::StageLevel];
  stageLevel.acc = true;
  stageLevel.defaultText = "50";
  stageLevel.name = "Level";
  stageLevel.slotCount = FFEnvStageCount;
  stageLevel.unit = "%";
  stageLevel.id = "{0E6A1B2B-AC57-473B-9B31-F67631C6B152}";
  stageLevel.type = FBParamType::Identity;
  auto selectStageLevel = [](auto& module) { return &module.acc.stageLevel; };
  stageLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageLevel);
  stageLevel.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStageLevel);
  stageLevel.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageLevel);
  stageLevel.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& stageSlope = result->params[(int)FFEnvParam::StageSlope];
  stageSlope.acc = true;
  stageSlope.defaultText = "50";
  stageSlope.name = "Slope";
  stageSlope.slotCount = FFEnvStageCount;
  stageSlope.unit = "%";
  stageSlope.id = "{A860A5DD-A18D-4B00-A394-53E4328642D2}";
  stageSlope.type = FBParamType::Identity;
  auto selectStageSlope = [](auto& module) { return &module.acc.stageSlope; };
  stageSlope.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageSlope);
  stageSlope.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStageSlope);
  stageSlope.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageSlope);
  stageSlope.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Exp }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& stageTime = result->params[(int)FFEnvParam::StageTime];
  stageTime.acc = false;
  stageTime.defaultText = "0";
  stageTime.name = "Time";
  stageTime.slotCount = FFEnvStageCount;
  stageTime.unit = "Sec";
  stageTime.id = "{3023BA36-07C3-422F-A894-5F6372603EEF}";
  stageTime.type = FBParamType::Linear;
  stageTime.Linear().min = 0.0f;
  stageTime.Linear().max = 10.0f;
  stageTime.Linear().editSkewFactor = 0.5f;
  auto selectStageTime = [](auto& module) { return &module.block.stageTime; };
  stageTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageTime);
  stageTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStageTime);
  stageTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageTime);
  stageTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  stageTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& stageBars = result->params[(int)FFEnvParam::StageBars];
  stageBars.acc = false;
  stageBars.defaultText = "Off";
  stageBars.name = "Bars";
  stageBars.slotCount = FFEnvStageCount;
  stageBars.unit = "Bars";
  stageBars.id = "{43780C3A-3C23-4A94-8BDF-152FDF408A5F}";
  stageBars.type = FBParamType::Bars;
  stageBars.Bars().items = MakeEnvBarsItems();
  auto selectStageBars = [](auto& module) { return &module.block.stageBars; };
  stageBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageBars);
  stageBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStageBars);
  stageBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageBars);
  stageBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  stageBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  return result;
}