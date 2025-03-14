#pragma once

#include <string>
#include <playground_base/base/topo/static/FBStaticParam.hpp>

int
FBMakeStableHash(std::string const& id);
std::string
FBMakeRuntimeName(std::string const& name, int slotCount, int slot, FBParamSlotFormatter formatter);
std::string
FBMakeRuntimeTooltip(std::string const& name, std::string const& tooltip, int slotCount, int slot, FBParamSlotFormatter formatter);