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
  return false;
}

void
FFDeserializationConverter::PostProcess(
  bool isGuiState,
  std::vector<double*> const& paramValues) const
{
  if (isGuiState)
    return;
  (void)paramValues;
}