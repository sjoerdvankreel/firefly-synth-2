#include <playground_plug/shared/FFOsciMod.hpp>

static std::map<std::pair<int, int>, int>
MakeSourceAndTargetToSlot();
static std::array<std::pair<int, int>, FFOsciModSlotCount>
MakeSlotToSourceAndTarget();

static std::map<std::pair<int, int>, int>
sourceAndTargetToSlot = MakeSourceAndTargetToSlot();
static std::array<std::pair<int, int>, FFOsciModSlotCount> 
slotToSourceAndTarget = MakeSlotToSourceAndTarget();

std::map<std::pair<int, int>, int> const&
FFOsciModSourceAndTargetToSlot()
{
  return sourceAndTargetToSlot;
}

std::array<std::pair<int, int>, FFOsciModSlotCount> const&
FFOsciModSlotToSourceAndTarget()
{
  return slotToSourceAndTarget;
}

static std::map<std::pair<int, int>, int>
MakeSourceAndTargetToSlot()
{
  int slot = 0;
  std::map<std::pair<int, int>, int> result = {};
  for (int tgt = 0; tgt < FFOsciCount; tgt++)
    for (int src = 0; src <= tgt; src++)
      result[{ src, tgt }] = slot++;
  return result;
}

static std::array<std::pair<int, int>, FFOsciModSlotCount>
MakeSlotToSourceAndTarget()
{
  int slot = 0;
  std::array<std::pair<int, int>, FFOsciModSlotCount> result;
  for (int tgt = 0; tgt < FFOsciCount; tgt++)
    for (int src = 0; src <= tgt; src++)
      result[slot++] = { src, tgt };
  return result;
}

std::string
FFOsciModMakeSourceAndTargetText(int slot)
{
  auto src = std::to_string(FFOsciModSlotToSourceAndTarget()[slot].first + 1);
  auto tgt = std::to_string(FFOsciModSlotToSourceAndTarget()[slot].second + 1);
  return src + ">" + tgt;
};