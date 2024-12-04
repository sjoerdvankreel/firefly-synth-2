#pragma once

#include <string>

int
FBMakeStableHash(std::string const& id);
std::string
FBMakeRuntimeName(std::string const& name, int slotCount, int slot);