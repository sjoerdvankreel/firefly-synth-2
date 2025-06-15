#pragma once

#include <string>
#include <firefly_base/base/topo/static/FBStaticParam.hpp>

int
FBMakeStableHash(
  std::string const& id);

std::string
FBMakeRuntimeTabName(
  std::string const& name,
  int slotCount, int slot);

std::string
FBMakeRuntimeGraphName(
  std::string const& name, 
  int slotCount, int slot);

std::string
FBMakeRuntimeShortName(
  std::string const& name, int slotCount, 
  int slot, FBParamSlotFormatter formatter);

std::string
FBMakeRuntimeDisplayName(
  std::string const& name, std::string const& display, 
  int slotCount, int slot, FBParamSlotFormatter formatter);