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
FBMakeRuntimeModuleShortName(
  FBStaticTopo const& topo, std::string const& name, 
  int slotCount, int slot,
  FBModuleSlotFormatter formatter, bool formatterOverrides);

std::string
FBMakeRuntimeModuleItemShortName(
  FBStaticTopo const& topo, std::string const& name,
  int moduleSlot, int itemSlotCount, int itemSlot,
  FBModuleItemSlotFormatter formatter, bool formatterOverrides);

std::string
FBMakeRuntimeModuleItemDisplayName(
  FBStaticTopo const& topo, std::string const& name, std::string const& display,
  int moduleSlot, int itemSlotCount, int itemSlot,
  FBModuleItemSlotFormatter formatter, bool formatterOverrides, bool slotFormatDisplay);