#pragma once

#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <string>

struct FBPlugVersion;
struct FBRuntimeTopo;

class FFDeserializationConverter final:
public FBDeserializationConverter
{
public:
  FFDeserializationConverter(
    FBPlugVersion const& oldVersion,
    FBRuntimeTopo const* topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFDeserializationConverter);

  bool OnParamNotFound(
    std::string const& oldModuleId, int oldModuleSlot,
    std::string const& oldParamId, int oldParamSlot,
    std::string& newModuleId, int& newModuleSlot,
    std::string& newParamId, int& newParamSlot) const override;
};