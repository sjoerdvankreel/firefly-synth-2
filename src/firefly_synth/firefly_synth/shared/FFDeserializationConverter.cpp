#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFDeserializationConverter.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

FFDeserializationConverter::
FFDeserializationConverter(
  FBPlugVersion const& oldVersion,
  FBRuntimeTopo const* topo):
FBDeserializationConverter(oldVersion, topo) {}

bool 
FFDeserializationConverter::OnParamNotFound(
  bool isGuiState,
  std::string const& oldModuleId, int oldModuleSlot,
  std::string const& oldParamId, int oldParamSlot,
  std::string& newModuleId, int& newModuleSlot,
  std::string& newParamId, int& newParamSlot) const
{
  if (isGuiState)
    return false;

  // 2.0.1 - added "V" prefix to per-voice effect module and params
  // NOTE TO SELF: this changed the param tags, so broke automation lanes.
  // Let's not ever do that again.
  if (OldVersion() < FBPlugVersion(2, 0, 1))
  {
    auto const& vEffectModule = Topo()->static_->modules[(int)FFModuleType::VEffect];
    if ("V" + oldModuleId == vEffectModule.id)
    {
      newModuleId = vEffectModule.id;
      for (int p = 0; p < vEffectModule.params.size(); p++)
      {
        auto const& vEffectParam = vEffectModule.params[p];
        if ("V" + oldParamId == vEffectParam.id)
        {
          newParamId = vEffectParam.id;
          newParamSlot = oldParamSlot;
          newModuleSlot = oldModuleSlot;
          return true;
        }
      }
    }
  }

  // 2.0.3 - changed FX oversample guid to fit outside of VST3 MIDI parameter range.
  // Block param, so no automation lanes.
  if (OldVersion() < FBPlugVersion(2, 0, 3))
  {
    auto const& vEffectModule = Topo()->static_->modules[(int)FFModuleType::VEffect];
    if (oldModuleId == vEffectModule.id && oldParamId == "V{28875DF7-255B-4190-80CE-D0A9ED20F263}")
    {
      newModuleId = oldModuleId;
      newParamSlot = oldParamSlot;
      newModuleSlot = oldModuleSlot;
      newParamId = "V{D8AA4B9D-EAFD-4E87-9DC9-108B8894A4D0}";
      return true;
    }
    auto const& gEffectModule = Topo()->static_->modules[(int)FFModuleType::GEffect];
    if (oldModuleId == gEffectModule.id && oldParamId == "G{28875DF7-255B-4190-80CE-D0A9ED20F263}")
    {
      newModuleId = oldModuleId;
      newParamSlot = oldParamSlot;
      newModuleSlot = oldModuleSlot;
      newParamId = "G{D8AA4B9D-EAFD-4E87-9DC9-108B8894A4D0}";
      return true;
    }
  }

  // 2.0.4 - added "G" prefix to global echo module and params
  // NOTE TO SELF: this changed the param tags, so broke automation lanes.
  // Let's REALLY not ever do that again.
  if (OldVersion() < FBPlugVersion(2, 0, 4))
  {
    auto const& gEchoModule = Topo()->static_->modules[(int)FFModuleType::GEcho];
    if ("G" + oldModuleId == gEchoModule.id)
    {
      newModuleId = gEchoModule.id;
      for (int p = 0; p < gEchoModule.params.size(); p++)
      {
        auto const& gEchoParam = gEchoModule.params[p];
        if ("G" + oldParamId == gEchoParam.id)
        {
          newParamId = gEchoParam.id;
          newParamSlot = oldParamSlot;
          newModuleSlot = oldModuleSlot;
          return true;
        }
      }
    }
  }

  return false;
}

void
FFDeserializationConverter::PostProcess(
  bool isGuiState,
  std::vector<double*> const& paramValues) const
{
  if (isGuiState)
    return;
  
  // 2.0.2 - Added dedicated amp envelope. That used to be env0, and since this
  // is a non-fixable breaking change, best we can do is just copy over env0 to ampEnv.
  if (OldVersion() < FBPlugVersion(2, 0, 2))
  {
    auto const& envModule = Topo()->static_->modules[(int)FFModuleType::Env];
    for (int p = 0; p < envModule.params.size(); p++)
    {
      auto const& envParam = envModule.params[p];
      for (int s = 0; s < envParam.slotCount; s++)
      {
        int rtEnv0ParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, 0 }, { p, s } })->runtimeParamIndex;
        int rtAmpEnvParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, FFAmpEnvSlot }, { p, s } })->runtimeParamIndex;
        *paramValues[rtAmpEnvParamIndex] = *paramValues[rtEnv0ParamIndex];
      }
    }
  }
}