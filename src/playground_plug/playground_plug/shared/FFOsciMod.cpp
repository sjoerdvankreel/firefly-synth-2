#include <playground_plug/shared/FFOsciMod.hpp>

static std::vector<std::vector<int>>
MakeSourceAndTargetToSlot();
static std::array<std::pair<int, int>, FFOsciModSlotCount>
MakeSlotToSourceAndTarget();

static std::vector<std::vector<int>>
sourceAndTargetToSlot = MakeSourceAndTargetToSlot();
static std::array<std::pair<int, int>, FFOsciModSlotCount> 
slotToSourceAndTarget = MakeSlotToSourceAndTarget();

std::vector<std::vector<int>> const&
FFOsciModSourceAndTargetToSlot()
{
  return sourceAndTargetToSlot;
}

std::array<std::pair<int, int>, FFOsciModSlotCount> const&
FFOsciModSlotToSourceAndTarget()
{
  return slotToSourceAndTarget;
}

static std::vector<std::vector<int>>
MakeSourceAndTargetToSlot()
{
  int slot = 0;
  std::vector<std::vector<int>> result = {};
  for (int tgt = 0; tgt < FFOsciCount; tgt++)
  {
    result.emplace_back();
    for (int src = 0; src <= tgt; src++)
      result[src].push_back(slot++);
  }
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
  auto src = std::to_string(slotToSourceAndTarget[slot].first + 1);
  auto tgt = std::to_string(slotToSourceAndTarget[slot].second + 1);
  return src + ">" + tgt;
};