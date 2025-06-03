#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGFilterTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GFilter";
  result->slotCount = FFGFilterCount;
  result->id = "{290E86EF-DFE9-4A3C-B6B2-9063643DD0E8}";
  result->params.resize((int)FFGFilterParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gFilter; });
  auto selectModule = [](auto& state) { return &state.global.gFilter; };

  auto& on = result->params[(int)FFGFilterParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{B9DF9543-5115-4D9C-89DD-62D5D495DBF8}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOn);
  on.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOn);
  on.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& mode = result->params[(int)FFGFilterParam::Mode];
  mode.acc = false;
  mode.defaultText = "LPF";
  mode.name = "Mode";
  mode.slotCount = 1;
  mode.id = "{503DECBB-EE24-4EC9-8AA2-DC865A38A70C}";
  mode.type = FBParamType::List;
  mode.List().items = {
    { "{7940E9B8-89DC-4795-AF4D-3A321F82AEF9}", "LPF" },
    { "{0BABF303-C235-45A7-A218-E1F05E3137F9}", "BPF" },
    { "{4939CA55-F119-412B-BF60-1DF803F0298C}", "HPF" },
    { "{C2D7EC53-95E5-49C6-8B99-AC7112618295}", "BSF" },
    { "{511DA538-55A1-427A-9935-9099961B1FB5}", "APF" },
    { "{F12556D5-B1D5-4E3D-A5EB-D22B072F942A}", "PEQ" },
    { "{77C474DC-8BCA-45A9-8017-CC19BFFD29B2}", "BLL" },
    { "{77FD4175-A9AD-4A2E-B701-C08477BCE07D}", "LSH" },
    { "{662A7E42-52D0-4069-AB74-1963E266D5A1}", "HSH" } };
  auto selectMode = [](auto& module) { return &module.block.mode; };
  mode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectMode);
  mode.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectMode);
  mode.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectMode);

  auto& res = result->params[(int)FFGFilterParam::Res];
  res.acc = true;
  res.defaultText = "0";
  res.name = "Res";
  res.slotCount = 1;
  res.unit = "%";
  res.id = "{ED140CF2-52C6-40A6-9F39-44E8069FFC77}";
  res.type = FBParamType::Identity;
  auto selectRes = [](auto& module) { return &module.acc.res; };
  res.scalarAddr = FFSelectScalarParamAddr(selectModule, selectRes);
  res.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectRes);
  res.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectRes);

  auto& freq = result->params[(int)FFGFilterParam::Freq];
  freq.acc = true;
  freq.defaultText = "1000";
  freq.name = "Freq";
  freq.slotCount = 1;
  freq.unit = "Hz";
  freq.id = "{24E988C5-7D41-4064-9212-111D1C3D2AF7}";
  freq.type = FBParamType::Log2;
  freq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectFreq = [](auto& module) { return &module.acc.freq; };
  freq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFreq);
  freq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFreq);
  freq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFreq);

  auto& gain = result->params[(int)FFGFilterParam::Gain];
  gain.acc = true;
  gain.defaultText = "0";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.unit = "dB";
  gain.id = "{8A4C5073-CE26-44CF-A244-425824596540}";
  gain.type = FBParamType::Linear;
  gain.Linear().min = -24.0f;
  gain.Linear().max = 24.0f;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectGain);
  gain.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectGain);
  gain.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectGain);
  gain.dependencies.enabled.audio.When({ (int)FFGFilterParam::Mode }, [](auto const& vs) { return vs[0] >= (int)FFStateVariableFilterMode::BLL; });

  return result;
}