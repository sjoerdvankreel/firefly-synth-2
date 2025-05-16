#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_plug/modules/noise/FFNoiseGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeNoiseTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Noise";
  result->slotCount = FFNoiseCount;
  result->graphCount = 1;
  result->graphRenderer = FFNoiseRenderGraph;
  result->id = "{3B4A03F2-0A04-425B-8BD5-D770AB4DC87E}";
  result->params.resize((int)FFNoiseParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.noise; });
  auto selectModule = [](auto& state) { return &state.voice.noise; };

  auto& type = result->params[(int)FFNoiseParam::Type];
  type.acc = false;
  type.defaultText = "Off";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{1C038085-4830-4985-9861-34B020BF85A3}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{75DEEBD4-15EF-402F-B092-D50C03B87737}", "Off" },
    { "{22D40A98-F62B-4022-A356-19AD460AE2B1}", "Uni" },
    { "{808C4DFA-F58D-44E9-AD82-CADF9C1C2A23}", "Norm" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);

  auto& gain = result->params[(int)FFNoiseParam::Gain];
  gain.acc = true;
  gain.defaultText = "100";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.unit = "%";
  gain.id = "{86CDFC86-DE6C-4EB0-BCE1-5A74A0488D4B}";
  gain.type = FBParamType::Identity;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectGain);
  gain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGain);

  auto& coarse = result->params[(int)FFNoiseParam::Coarse];
  coarse.acc = true;
  coarse.defaultText = "0";
  coarse.name = "Coarse";
  coarse.slotCount = 1;
  coarse.unit = "Semitones";
  coarse.id = "{AD56E1E7-CB97-4C67-8DC5-9CF74D3D7C4A}";
  coarse.type = FBParamType::Linear;
  coarse.Linear().min = -12.0f;
  coarse.Linear().max = 12.0f;
  auto selectCoarse = [](auto& module) { return &module.acc.coarse; };
  coarse.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCoarse);
  coarse.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCoarse);
  coarse.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCoarse);

  auto& fine = result->params[(int)FFNoiseParam::Fine];
  fine.acc = true;
  fine.defaultText = "0";
  fine.name = "Fine";
  fine.slotCount = 1;
  fine.unit = "Cent";
  fine.id = "{8BA313C0-450C-4A25-9D00-1219B325D7DD}";
  fine.type = FBParamType::Linear;
  fine.Linear().min = -1.0f;
  fine.Linear().max = 1.0f;
  fine.Linear().displayMultiplier = 100.0f;
  auto selectFine = [](auto& module) { return &module.acc.fine; };
  fine.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFine);
  fine.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectFine);
  fine.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFine);

  auto& uniCount = result->params[(int)FFNoiseParam::UniCount];
  uniCount.acc = false;
  uniCount.defaultText = "1";
  uniCount.display = "Uni";
  uniCount.name = "Unison Count";
  uniCount.slotCount = 1;
  uniCount.id = "{CB0CD524-CD0F-4150-8DE7-A2DD9E12EB37}";
  uniCount.type = FBParamType::Discrete;
  uniCount.Discrete().valueCount = FFOsciBaseUniMaxCount;
  uniCount.Discrete().valueOffset = 1;
  auto selectUniCount = [](auto& module) { return &module.block.uniCount; };
  uniCount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniCount);
  uniCount.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectUniCount);
  uniCount.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniCount);

  auto& uniDetune = result->params[(int)FFNoiseParam::UniDetune];
  uniDetune.acc = true;
  uniDetune.defaultText = "33";
  uniDetune.display = "Dtn";
  uniDetune.name = "Unison Detune";
  uniDetune.slotCount = 1;
  uniDetune.unit = "%";
  uniDetune.id = "{086DD556-6CC5-4E52-9A06-4B55FF59AE10}";
  uniDetune.type = FBParamType::Identity;
  auto selectUniDetune = [](auto& module) { return &module.acc.uniDetune; };
  uniDetune.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniDetune);
  uniDetune.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniDetune);
  uniDetune.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniDetune);
  uniDetune.dependencies.enabled.audio.When({ (int)FFNoiseParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& uniSpread = result->params[(int)FFNoiseParam::UniSpread];
  uniSpread.acc = true;
  uniSpread.defaultText = "50";
  uniSpread.display = "Sprd";
  uniSpread.name = "Unison Spread";
  uniSpread.slotCount = 1;
  uniSpread.unit = "%";
  uniSpread.id = "{DC396041-B75D-45C2-9892-00699B15D498}";
  uniSpread.type = FBParamType::Identity;
  auto selectUniSpread = [](auto& module) { return &module.acc.uniSpread; };
  uniSpread.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniSpread);
  uniSpread.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniSpread);
  uniSpread.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniSpread);
  uniSpread.dependencies.enabled.audio.When({ (int)FFNoiseParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& uniBlend = result->params[(int)FFNoiseParam::UniBlend];
  uniBlend.acc = true;
  uniBlend.defaultText = "100";
  uniBlend.display = "Blnd";
  uniBlend.name = "Unison Blend";
  uniBlend.slotCount = 1;
  uniBlend.unit = "%";
  uniBlend.id = "{DB12942B-8978-4877-B37D-D5AB5E4A536A}";
  uniBlend.type = FBParamType::Identity;
  auto selectUniBlend = [](auto& module) { return &module.acc.uniBlend; };
  uniBlend.scalarAddr = FFSelectScalarParamAddr(selectModule, selectUniBlend);
  uniBlend.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectUniBlend);
  uniBlend.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectUniBlend);
  uniBlend.dependencies.enabled.audio.When({ (int)FFNoiseParam::UniCount }, [](auto const& vs) { return vs[0] != 1; });

  auto& seed = result->params[(int)FFNoiseParam::Seed];
  seed.acc = false;
  seed.defaultText = "0";
  seed.name = "Seed";
  seed.slotCount = 1;
  seed.id = "{147FBBB6-3C3B-471B-8BDA-A10DA068769E}";
  seed.type = FBParamType::Discrete;
  seed.Discrete().valueCount = FFNoiseMaxSeed + 1;
  auto selectSeed = [](auto& module) { return &module.block.seed; };
  seed.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSeed);
  seed.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSeed);
  seed.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSeed);

  auto& poles = result->params[(int)FFNoiseParam::Poles];
  poles.acc = false;
  poles.defaultText = "4";
  poles.name = "Q";
  poles.slotCount = 1;
  poles.id = "{84B10EBF-E55D-43DF-8E80-3F1FCE093400}";
  poles.type = FBParamType::Discrete;
  poles.Discrete().valueOffset = 1;
  poles.Discrete().valueCount = FFNoiseMaxPoles;
  auto selectPoles = [](auto& module) { return &module.block.poles; };
  poles.scalarAddr = FFSelectScalarParamAddr(selectModule, selectPoles);
  poles.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectPoles);
  poles.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectPoles);

  auto& color = result->params[(int)FFNoiseParam::Color];
  color.acc = true;
  color.defaultText = "50";
  color.name = "Color";
  color.slotCount = 1;
  color.unit = "%";
  color.id = "{32C65288-B8CD-4245-9ED1-818667C65253}";
  color.type = FBParamType::Identity;
  auto selectColor = [](auto& module) { return &module.acc.color; };
  color.scalarAddr = FFSelectScalarParamAddr(selectModule, selectColor);
  color.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectColor);
  color.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectColor);

  auto& x = result->params[(int)FFNoiseParam::X];
  x.acc = true;
  x.defaultText = "100";
  x.name = "X";
  x.slotCount = 1;
  x.unit = "%";
  x.id = "{E911D100-EAD7-4915-AB3C-A27BA73396EC}";
  x.type = FBParamType::Identity;
  auto selectX = [](auto& module) { return &module.acc.x; };
  x.scalarAddr = FFSelectScalarParamAddr(selectModule, selectX);
  x.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectX);
  x.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectX);

  auto& y = result->params[(int)FFNoiseParam::Y];
  y.acc = true;
  y.defaultText = "100";
  y.name = "Y";
  y.slotCount = 1;
  y.unit = "%";
  y.id = "{1979C7C6-F86E-40EE-8245-163C9F497469}";
  y.type = FBParamType::Identity;
  auto selectY = [](auto& module) { return &module.acc.y; };
  y.scalarAddr = FFSelectScalarParamAddr(selectModule, selectY);
  y.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectY);
  y.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectY);

  return result;
}