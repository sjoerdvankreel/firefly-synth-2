#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFDeserializationConverter.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/mix/FFGMixTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
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

void
FFDeserializationConverter::PostProcess(
  bool isGuiState,
  std::vector<FBOldParamInfo> const& oldState,
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

  // 2.0.8 - osci pan now displays as bipolar.
  if (OldVersion() < FBPlugVersion(2, 0, 8))
  {      
    auto const& osciModule = Topo()->static_->modules[(int)FFModuleType::Osci];
    for (int ms = 0; ms < osciModule.slotCount; ms++)
    {
      int rtParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Osci, ms }, { (int)FFOsciParam::Pan, 0 } })->runtimeParamIndex;
      *paramValues[rtParamIndex] = FBToBipolar(*paramValues[rtParamIndex]);
    }
  }

  // 2.1.0 - add new soft clip param.
  // The default is 12dB but the old "default" was off because not exist.
  if (OldVersion() < FBPlugVersion(2, 1, 0))
  {
    auto const& settingsModule = Topo()->static_->modules[(int)FFModuleType::Settings];
    int rtParamIndex = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::AutoSoftClip, 0 } })->runtimeParamIndex;
    auto offVal = settingsModule.params[(int)FFSettingsParam::AutoSoftClip].TextToNormalized(false, (int)FFModuleType::Settings, "Off");
    if (offVal.has_value())
      *paramValues[rtParamIndex] = offVal.value();
  }

  // 2.1.1 - clamped down matrix routes
  if (OldVersion() < FBPlugVersion(2, 1, 1))
  {
    auto voiceParam = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::VMatrix, 0 }, { (int)FFModMatrixParam::Slots, 0 } });
    auto globalParam = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::GMatrix, 0 }, { (int)FFModMatrixParam::Slots, 0 } });
    for (int i = 0; i < oldState.size(); i++)
    {
      if (oldState[i].id == voiceParam->id)
      {
        int oldSlots = std::stoi(oldState[i].value);
        if (oldSlots > FFModMatrixVoiceMaxSlotCount)
          *paramValues[voiceParam->runtimeParamIndex] = 1.0;
      }
      if (oldState[i].id == globalParam->id)
      {
        int oldSlots = std::stoi(oldState[i].value);
        if (oldSlots > FFModMatrixGlobalMaxSlotCount)
          *paramValues[globalParam->runtimeParamIndex] = 1.0;
      }
    }
  }

  // 2.1.1 - dropped some global uni targets
  if (OldVersion() < FBPlugVersion(2, 1, 1))
  {
    auto const& globalUniModule = Topo()->static_->modules[(int)FFModuleType::GlobalUni];
    auto const& modeParam = globalUniModule.params[(int)FFGlobalUniParam::Mode];
    auto const& opTypeParam = globalUniModule.params[(int)FFGlobalUniParam::OpType];
    auto const& autoSpreadParam = globalUniModule.params[(int)FFGlobalUniParam::AutoSpread];
    auto const& autoSkewParam = globalUniModule.params[(int)FFGlobalUniParam::AutoSkew];
    auto const& autoRandParam = globalUniModule.params[(int)FFGlobalUniParam::AutoRand];
    auto const& autoSeedParam = globalUniModule.params[(int)FFGlobalUniParam::AutoRandSeed];
    auto const& autoFreeParam = globalUniModule.params[(int)FFGlobalUniParam::AutoRandFree];

    auto applyIfSlotMatch = [this, &globalUniModule, &paramValues](
        auto const& oldSlotState, int paramIndex, int oldSlot, int newSlot) 
    {
      auto const& param = globalUniModule.params[paramIndex];
      if (oldSlotState.paramSlot == oldSlot)
      {
        auto oldVal = param.TextToNormalized(true, 0, oldSlotState.value);
        if(oldVal.has_value())
          *paramValues[Topo()->audio.paramTopoToRuntime[(int)FFModuleType::GlobalUni][0][paramIndex][newSlot]] = oldVal.value();
      }
    };

    auto applyAllIfSlotMatch = [&applyIfSlotMatch](auto const& oldSlotState, int paramIndex) {
      applyIfSlotMatch(oldSlotState, paramIndex, 14, 12);
      applyIfSlotMatch(oldSlotState, paramIndex, 15, 13);
      applyIfSlotMatch(oldSlotState, paramIndex, 16, 14);
      applyIfSlotMatch(oldSlotState, paramIndex, 17, 15);
      applyIfSlotMatch(oldSlotState, paramIndex, 18, 16);
      applyIfSlotMatch(oldSlotState, paramIndex, 19, 17);
      applyIfSlotMatch(oldSlotState, paramIndex, 20, 18);
      applyIfSlotMatch(oldSlotState, paramIndex, 23, 19);
      applyIfSlotMatch(oldSlotState, paramIndex, 24, 20);
      applyIfSlotMatch(oldSlotState, paramIndex, 25, 21);
      applyIfSlotMatch(oldSlotState, paramIndex, 28, 22);
      applyIfSlotMatch(oldSlotState, paramIndex, 29, 23);
      applyIfSlotMatch(oldSlotState, paramIndex, 30, 24);
      applyIfSlotMatch(oldSlotState, paramIndex, 33, 25);
      applyIfSlotMatch(oldSlotState, paramIndex, 34, 26);
      applyIfSlotMatch(oldSlotState, paramIndex, 35, 27);
    };

    for (int i = 0; i < oldState.size(); i++)
    {
      if (oldState[i].moduleId == globalUniModule.id)
      {
        if (oldState[i].paramId == modeParam.id)
          applyAllIfSlotMatch(oldState[i], (int)FFGlobalUniParam::Mode);
        if (oldState[i].paramId == opTypeParam.id)
          applyAllIfSlotMatch(oldState[i], (int)FFGlobalUniParam::OpType);
        if (oldState[i].paramId == autoSpreadParam.id)
          applyAllIfSlotMatch(oldState[i], (int)FFGlobalUniParam::AutoSpread);
        if (oldState[i].paramId == autoSkewParam.id)
          applyAllIfSlotMatch(oldState[i], (int)FFGlobalUniParam::AutoSkew);
        if (oldState[i].paramId == autoRandParam.id)
          applyAllIfSlotMatch(oldState[i], (int)FFGlobalUniParam::AutoRand);
        if (oldState[i].paramId == autoSeedParam.id)
          applyAllIfSlotMatch(oldState[i], (int)FFGlobalUniParam::AutoRandSeed);
        if (oldState[i].paramId == autoFreeParam.id)
          applyAllIfSlotMatch(oldState[i], (int)FFGlobalUniParam::AutoRandFree);
      }
    }
  }

  // 2.1.1 - moved SVF mode into effect kind.
  if (OldVersion() < FBPlugVersion(2, 1, 1))
  {
    std::map<std::string, std::string> svModeToEffectKind = {};
    std::array<std::array<std::string, FFEffectBlockCount>, FFEffectCount> vOldKind = {};
    std::array<std::array<std::string, FFEffectBlockCount>, FFEffectCount> gOldKind = {};
    std::array<std::array<std::string, FFEffectBlockCount>, FFEffectCount> vOldStVarMode = {};
    std::array<std::array<std::string, FFEffectBlockCount>, FFEffectCount> gOldStVarMode = {};
    svModeToEffectKind["{EAAE7102-9F6C-4EC2-8C39-B13BBDFF7AD1}"] = "{14948CA0-469E-4F04-898E-BAD4B92D50AB}";
    svModeToEffectKind["{6A91C381-DB9F-4CAA-8155-4A407700661A}"] = "{5AE1ED7B-9F20-4822-92F1-32FCE60946D8}";
    svModeToEffectKind["{747DA91C-C43D-4CFC-8EFD-353B0AC23E0E}"] = "{32FE4541-7D8F-452D-8878-9CD632A0A324}";
    svModeToEffectKind["{10FEE670-AB90-4DBF-A617-6F15F3E4602D}"] = "{955E2ED1-0B2E-4E31-A7EE-E6CBBD388B54}";
    svModeToEffectKind["{EE9A4F79-B557-43B4-ABDF-320414D773D5}"] = "{1DCB92CA-92C0-4C0E-80CC-B8AC03343ECD}";
    svModeToEffectKind["{C9C3A3F5-5C5B-4331-8F6E-DDD2DC5A1D7B}"] = "{4038CFB6-5FEF-4C9B-9C44-098E7090918D}";
    svModeToEffectKind["{8D885EEB-DF69-455A-9FFD-BA95E3E30596}"] = "{6209AC05-A0AF-4322-956B-0F875335BD27}";
    svModeToEffectKind["{C4BBC616-CFDB-4E93-9315-D25552D85F71}"] = "{2BD0BBBA-616D-422E-949E-8A82514C3B13}";
    svModeToEffectKind["{AF2550ED-90C5-4E09-996D-A4669728C744}"] = "{9A047C20-0F36-4931-BD57-3FF6E2C256CA}";

    for (int i = 0; i < oldState.size(); i++)
    {
      if (oldState[i].moduleId == "G{154051CE-66D9-41C8-B479-C52D1111C962}") // gfx
        if (oldState[i].paramId == "G{1585A19D-639E-4202-B60B-BD8560BC8B70}") // kind
          if (oldState[i].moduleSlot < FFEffectCount)
            if (oldState[i].paramSlot < FFEffectBlockCount)
              gOldKind[oldState[i].moduleSlot][oldState[i].paramSlot] = oldState[i].value;

      if (oldState[i].moduleId == "G{154051CE-66D9-41C8-B479-C52D1111C962}") // gfx
        if (oldState[i].paramId == "G{275B2C8D-6D21-4741-AB69-D21FA95CD7F5}") // sv mode
          if (oldState[i].moduleSlot < FFEffectCount)
            if (oldState[i].paramSlot < FFEffectBlockCount)
              gOldStVarMode[oldState[i].moduleSlot][oldState[i].paramSlot] = oldState[i].value;

      if (oldState[i].moduleId == "V{154051CE-66D9-41C8-B479-C52D1111C962}") // vfx
        if (oldState[i].paramId == "V{1585A19D-639E-4202-B60B-BD8560BC8B70}") // kind
          if (oldState[i].moduleSlot < FFEffectCount)
            if (oldState[i].paramSlot < FFEffectBlockCount)
              vOldKind[oldState[i].moduleSlot][oldState[i].paramSlot] = oldState[i].value;

      if (oldState[i].moduleId == "V{154051CE-66D9-41C8-B479-C52D1111C962}") // vfx
        if (oldState[i].paramId == "V{275B2C8D-6D21-4741-AB69-D21FA95CD7F5}") // sv mode
          if (oldState[i].moduleSlot < FFEffectCount)
            if (oldState[i].paramSlot < FFEffectBlockCount)
              vOldStVarMode[oldState[i].moduleSlot][oldState[i].paramSlot] = oldState[i].value;
    }

    std::map<std::string, std::string>::iterator iter;
    for (int i = 0; i < FFEffectCount; i++)
      for (int j = 0; j < FFEffectBlockCount; j++)
      {
        if(gOldKind[i][j] == "{348FED12-9753-4C48-9D21-BB8D21E036AB}") // svf
          if ((iter = svModeToEffectKind.find(gOldStVarMode[i][j])) != svModeToEffectKind.end())
          {
            auto const* param = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::GEffect, i }, { (int)FFEffectParam::Kind, j } });
            auto norm = param->TextToNormalized(true, iter->second);
            if (norm.has_value())
              *paramValues[param->runtimeParamIndex] = norm.value();
          }
        if (vOldKind[i][j] == "{348FED12-9753-4C48-9D21-BB8D21E036AB}") // svf
          if ((iter = svModeToEffectKind.find(vOldStVarMode[i][j])) != svModeToEffectKind.end())
          {
            auto const* param = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::VEffect, i }, { (int)FFEffectParam::Kind, j } });
            auto norm = param->TextToNormalized(true, iter->second);
            if (norm.has_value())
              *paramValues[param->runtimeParamIndex] = norm.value();
          }
      }
  }

  // 2.1.2 - allow sidechaining for fx
  if (OldVersion() < FBPlugVersion(2, 1, 2))
  {
    if (!Topo()->static_->meta.isFx)
    {
      auto const& gMixModule = Topo()->static_->modules[(int)FFModuleType::GMix];
      auto const& audioInToOut = gMixModule.params[(int)FFGMixParam::AudioInToOut];
      auto const& audioInToGFX = gMixModule.params[(int)FFGMixParam::AudioInToGFX];
      auto const& sidechainToGFX = gMixModule.params[(int)FFGMixParam::SidechainToGFX];
      for (int i = 0; i < oldState.size(); i++)
      {
        if (oldState[i].moduleId == gMixModule.id)
        {
          if (oldState[i].paramId == audioInToOut.id)
          {
            auto const* param = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::SidechainToOut, 0 } });
            auto norm = param->TextToNormalized(true, oldState[i].value);
            if (norm.has_value())
              *paramValues[param->runtimeParamIndex] = norm.value();
          }
          if (oldState[i].paramId == audioInToGFX.id && oldState[i].paramSlot < sidechainToGFX.slotCount)
          {
            auto const* param = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::SidechainToGFX, oldState[i].paramSlot}});
            auto norm = param->TextToNormalized(true, oldState[i].value);
            if (norm.has_value())
              *paramValues[param->runtimeParamIndex] = norm.value();
          }
        }
        auto const* oldToOutParam = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::AudioInToOut, 0 } });
        *paramValues[oldToOutParam->runtimeParamIndex] = 0.0;
        for (int j = 0; j < audioInToGFX.slotCount; j++)
        {
          auto const* oldToGFXParam = Topo()->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::AudioInToGFX, j } });
          *paramValues[oldToGFXParam->runtimeParamIndex] = 0.0;
        }
      }
    }
  }
}