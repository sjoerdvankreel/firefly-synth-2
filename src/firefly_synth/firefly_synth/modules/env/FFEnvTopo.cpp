#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/env/FFEnvGraph.hpp>

#include <firefly_base/base/topo/static/FBStaticModule.hpp>

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
  result->graphCount = FFEnvCount;
  result->slotFormatterOverrides = true;
  result->graphRenderer = FFEnvRenderGraph;
  result->id = "{FC1DC75A-200C-4465-8CBE-0100E2C8FAF2}";
  result->params.resize((int)FFEnvParam::Count);
  result->cvOutputs.resize((int)FFEnvCVOutput::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.env; });
  result->tabSlotFormatter = [](FBStaticTopo const&, int s) { return s == FFAmpEnvSlot ? "Amp" : std::to_string(s); };
  result->slotFormatter = [](FBStaticTopo const&, int s) { return s == FFAmpEnvSlot ? "Amp Env" : "Env " + std::to_string(s); };
  auto selectModule = [](auto& state) { return &state.voice.env; };

  auto& type = result->params[(int)FFEnvParam::Type];
  type.mode = FBParamMode::Block;
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{40F1DCF0-03B1-4ABE-B6B5-A29BF2C8C229}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{6F0DA153-9544-4EFB-BC6D-88F761583F39}", "Off" },
    { "{BD01A08E-5639-4DB3-87CD-3276BCDB54E1}", "Linear" },
    { "{30BF083A-81F1-477C-BC6B-5AA4DFB111A8}", "Exp" } };
  type.defaultTextSelector = [](int /*mi*/, int ms, int /*ps*/) { return ms == FFAmpEnvSlot ? "Linear" : "Off"; }; 
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);

  auto& sync = result->params[(int)FFEnvParam::Sync];
  sync.mode = FBParamMode::Block;
  sync.name = "Tempo Sync";
  sync.display = "Sync";
  sync.slotCount = 1;
  sync.id = "{B1128167-2EB6-4AFA-81B7-C4B7606502BB}";
  sync.defaultText = "Off";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSync);
  sync.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSync);
  sync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSync);
  sync.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& release = result->params[(int)FFEnvParam::Release];
  release.mode = FBParamMode::Block;
  release.defaultText = "2";
  release.name = "Release";
  release.slotCount = 1;
  release.id = "{38670133-4372-461F-ACB8-0E1E156BD3DF}";
  release.type = FBParamType::Discrete;
  release.Discrete().valueCount = FFEnvStageCount + 1;
  release.Discrete().valueFormatter = [](int v) { return v == 0 ? "Off" : std::to_string(v); };
  auto selectRelease = [](auto& module) { return &module.block.release; };
  release.scalarAddr = FFSelectScalarParamAddr(selectModule, selectRelease);
  release.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectRelease);
  release.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectRelease);
  release.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& loopStart = result->params[(int)FFEnvParam::LoopStart];
  loopStart.mode = FBParamMode::Block;
  loopStart.defaultText = "2";
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
  loopStart.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& loopLength = result->params[(int)FFEnvParam::LoopLength];
  loopLength.mode = FBParamMode::Block;
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
  loopLength.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type, (int)FFEnvParam::LoopStart }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& smoothTime = result->params[(int)FFEnvParam::SmoothTime];
  smoothTime.mode = FBParamMode::Block;
  smoothTime.defaultText = "0";
  smoothTime.display = "Smooth";
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
  smoothTime.dependencies.visible.audio.WhenSimple({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  smoothTime.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& smoothBars = result->params[(int)FFEnvParam::SmoothBars];
  smoothBars.mode = FBParamMode::Block;
  smoothBars.defaultText = "Off";
  smoothBars.display = "Smooth";
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
  smoothBars.dependencies.visible.audio.WhenSimple({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  smoothBars.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& stageLevel = result->params[(int)FFEnvParam::StageLevel];
  stageLevel.mode = FBParamMode::Accurate;
  stageLevel.name = "Level";
  stageLevel.slotCount = FFEnvStageCount;
  stageLevel.unit = "%";
  stageLevel.id = "{0E6A1B2B-AC57-473B-9B31-F67631C6B152}";
  stageLevel.defaultTextSelector = [](int /*mi*/, int /*ms*/, int ps) { return ps == 0 ? "100" : ps == 1 ? "25" : "0"; };
  stageLevel.type = FBParamType::Identity;
  auto selectStageLevel = [](auto& module) { return &module.acc.stageLevel; };
  stageLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageLevel);
  stageLevel.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStageLevel);
  stageLevel.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageLevel);
  stageLevel.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& stageSlope = result->params[(int)FFEnvParam::StageSlope];
  stageSlope.mode = FBParamMode::Accurate;
  stageSlope.defaultText = "0";
  stageSlope.name = "Slope";
  stageSlope.slotCount = FFEnvStageCount;
  stageSlope.unit = "%";
  stageSlope.id = "{A860A5DD-A18D-4B00-A394-53E4328642D2}";
  stageSlope.type = FBParamType::Identity;
  stageSlope.Identity().displayAsBipolar = true;
  auto selectStageSlope = [](auto& module) { return &module.acc.stageSlope; };
  stageSlope.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageSlope);
  stageSlope.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStageSlope);
  stageSlope.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageSlope);
  stageSlope.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEnvType::Exp; });

  auto& stageTime = result->params[(int)FFEnvParam::StageTime];
  stageTime.mode = FBParamMode::VoiceStart;
  stageTime.name = "Time";
  stageTime.slotCount = FFEnvStageCount;
  stageTime.unit = "Sec";
  stageTime.id = "{3023BA36-07C3-422F-A894-5F6372603EEF}";
  stageTime.defaultTextSelector = [](int /*mi*/, int /*ms*/, int ps) { return ps == 0 ? "0.1" : ps == 1 ? "0.2" : ps == 2 ? "0.4" : "0"; };
  stageTime.type = FBParamType::Linear;
  stageTime.Linear().min = 0.0f;
  stageTime.Linear().max = 10.0f;
  stageTime.Linear().editSkewFactor = 0.5f;
  auto selectStageTime = [](auto& module) { return &module.voiceStart.stageTime; };
  stageTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageTime);
  stageTime.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStageTime);
  stageTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageTime);
  stageTime.dependencies.visible.audio.WhenSimple({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  stageTime.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& stageBars = result->params[(int)FFEnvParam::StageBars];
  stageBars.mode = FBParamMode::Block;
  stageBars.name = "Bars";
  stageBars.slotCount = FFEnvStageCount;
  stageBars.unit = "Bars";
  stageBars.id = "{43780C3A-3C23-4A94-8BDF-152FDF408A5F}";
  stageBars.defaultTextSelector = [](int /*mi*/, int /*ms*/, int ps) { return ps == 0 ? "1/16" : ps == 1 ? "1/8" : ps == 2 ? "1/4" : "Off"; };
  stageBars.type = FBParamType::Bars;
  stageBars.Bars().items = MakeEnvBarsItems();
  auto selectStageBars = [](auto& module) { return &module.block.stageBars; };
  stageBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStageBars);
  stageBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStageBars);
  stageBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStageBars);
  stageBars.dependencies.visible.audio.WhenSimple({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  stageBars.dependencies.enabled.audio.WhenSimple({ (int)FFEnvParam::Type, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& output = result->cvOutputs[(int)FFEnvCVOutput::Output];
  output.slotCount = 1;
  output.name = "Output";
  output.id = "{4C86B531-321C-46C7-AF9C-D13B92572F48}";
  output.voiceAddr = [](int ms, int, int voice, void* state) { return &static_cast<FFProcState*>(state)->dsp.voice[voice].env[ms].output; };

  return result;
}