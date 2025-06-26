#include <firefly_synth/shared/FFDeserializationConverter.hpp>

FFDeserializationConverter::
FFDeserializationConverter(
  FBPlugVersion const& oldVersion,
  FBRuntimeTopo const* topo):
FBDeserializationConverter(oldVersion, topo) {}

bool 
FFDeserializationConverter::OnParamNotFound(
  std::string const& /*oldModuleId*/, int /*oldModuleSlot*/,
  std::string const& /*oldParamId*/, int /*oldParamSlot*/,
  std::string& /*newModuleId*/, int& /*newModuleSlot*/,
  std::string& /*newParamId*/, int& /*newParamSlot*/) const
{
  return false;
}