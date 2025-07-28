#pragma once

struct FBStaticTopo;

#include <string>

#include <firefly_base/base/topo/static/FBStaticParam.hpp>
#include <firefly_base/base/topo/static/FBSlotFormatter.hpp>

int
FBMakeStableHash(
  std::string const& id);

std::string
FBMakeRuntimeId(
  std::string const& staticModuleId, int moduleSlot,
  std::string const& staticId, int slot); 

std::string
FBMakeRuntimeShortName(
  FBStaticTopo const& topo, std::string const& name, 
  int slotCount, int slot, 
  FBSlotFormatter formatter, bool formatterOverrides);

std::string
FBMakeRuntimeDisplayName(
  FBStaticTopo const& topo, std::string const& name,
  std::string const& display, int slotCount, int slot, 
  FBSlotFormatter formatter, bool formatterOverrides);