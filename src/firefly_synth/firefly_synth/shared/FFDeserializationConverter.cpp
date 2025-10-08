#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFDeserializationConverter.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
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

  // 2.0.5 - Upped lfo and env count from 4/5 to 6/7, and moved amp env from last slot to first.
  // This conversion will not catch everything, in particular not host automation and internal modulation matrix.
  // The conversion itself is to shift env0...3 to 1..4 and move 4 to 0. We have 2 new free slots to use as scratch space.
  if (OldVersion() < FBPlugVersion(2, 0, 5))
  {
    auto const& envModule = Topo()->static_->modules[(int)FFModuleType::Env];

    // shift all to the right
    for (int i = 4; i >= 0; i--)
    {
      if (envModule.slotCount > i + 1)
      {
        for (int p = 0; p < envModule.params.size(); p++)
        {
          auto const& envParam = envModule.params[p];
          for (int s = 0; s < envParam.slotCount; s++)
          {
            int rtOldParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, i }, { p, s } })->runtimeParamIndex;
            int rtNewParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, i + 1 }, { p, s } })->runtimeParamIndex;
            *paramValues[rtNewParamIndex] = *paramValues[rtOldParamIndex];
          }
        }
      }
    }

    // slot 5 is now what used to be amp env, copy over to 0 and clear
    if (envModule.slotCount > 5)
    {
      for (int p = 0; p < envModule.params.size(); p++)
      {
        auto const& envParam = envModule.params[p];
        for (int s = 0; s < envParam.slotCount; s++)
        {
          int rtOldParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, 5 }, { p, s } })->runtimeParamIndex;
          int rtNewParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, 0 }, { p, s } })->runtimeParamIndex;
          *paramValues[rtNewParamIndex] = *paramValues[rtOldParamIndex];
          *paramValues[rtOldParamIndex] = envParam.DefaultNormalizedByText((int)FFModuleType::Env, 5, s);
        }
      }
    }
  }

  // 2.0.6 - envelope slope now displays as bipolar.
  if (OldVersion() < FBPlugVersion(2, 0, 6))
  {
    auto const& envModule = Topo()->static_->modules[(int)FFModuleType::Env];
    auto const& slopeParam = envModule.params[(int)FFEnvParam::StageSlope];
    for (int ms = 0; ms < envModule.slotCount; ms++)
      for (int ps = 0; ps < slopeParam.slotCount; ps++)
      {
        int rtParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, ms }, { (int)FFEnvParam::StageSlope, ps } })->runtimeParamIndex;
        *paramValues[rtParamIndex] = FBToBipolar(*paramValues[rtParamIndex]);
      }
  }
}

bool 
FFDeserializationConverter::OnParamListItemNotFound(
  bool isGuiState,
  std::string const& moduleId, int /*moduleSlot*/,
  std::string const& paramId, int /*paramSlot*/,
  std::string const& oldParamValue, std::string& newParamValue) const
{
  if (isGuiState)
    return false;

  // 2.0.4 - added "G" prefix to global echo module and params
  // NOTE TO SELF: this changed the mod matrix list item values, so broke modulation too.
  // Let's REALLY not ever do that again.
  if (OldVersion() < FBPlugVersion(2, 0, 4))
  {
    auto const& gEchoModule = Topo()->static_->modules[(int)FFModuleType::GEcho];
    auto const& gMatrixModule = Topo()->static_->modules[(int)FFModuleType::GMatrix];
    if (moduleId == gMatrixModule.id)
    {
      auto const& gMatrixTargetParam = gMatrixModule.params[(int)FFModMatrixParam::Target];
      if (paramId == gMatrixTargetParam.id)
      {
        // {guid}-slot-{guid}-{slot}
        if (oldParamValue.size() == 81)
        {
          std::string moduleGuid = oldParamValue.substr(0, 38);
          if ("G" + moduleGuid == gEchoModule.id)
          {
            std::string paramGuid = oldParamValue.substr(41, 38);
            for (int p = 0; p < (int)FFEchoParam::Count; p++)
            {
              auto const& gEchoParam = gEchoModule.params[p];
              if ("G" + paramGuid == gEchoParam.id)
              {
                std::string paramSlot = oldParamValue.substr(80, 1);
                std::string moduleSlot = oldParamValue.substr(39, 1);
                newParamValue = "G" + moduleGuid + "-" + moduleSlot + "-G" + paramGuid + "-" + paramSlot;
                return true;
              }
            }
          }
        }
      }
    }
  }

  return false;
}