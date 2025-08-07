#pragma once

#include <string>
#include <functional>

struct FBStaticTopo;

typedef std::function<std::string(FBStaticTopo const& topo, int moduleSlot)>
FBModuleSlotFormatter;
typedef std::function<std::string(FBStaticTopo const& topo, int moduleSlot, int itemSlot)>
FBModuleItemSlotFormatter;