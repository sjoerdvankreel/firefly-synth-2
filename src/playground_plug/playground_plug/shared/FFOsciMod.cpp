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

static std::string
FFOsciModMakeSourceAndTargetText(int slot)
{
  auto sourceAndTarget = MakeSlotToSourceAndTarget()[slot];
  return std::to_string(sourceAndTarget.first + 1) + ">" + std::to_string(sourceAndTarget.second + 1);
};

static std::vector<std::vector<int>>
MakeSourceAndTargetToSlot()
{
  int i = 0;
  std::vector<std::vector<int>> result = {};
  for (int j = 0; j < FFOsciCount; j++)
  {
    result.emplace_back();
    for (int k = 0; k <= j; k++)
      result[j].push_back(i++);
  }
  return result;
}

static std::array<std::pair<int, int>, FFOsciModSlotCount>
MakeSlotToSourceAndTarget()
{
  int i = 0;
  std::array<std::pair<int, int>, FFOsciModSlotCount> result;
  for (int j = 0; j < FFOsciCount; j++)
    for (int k = 0; k <= j; k++)
      result[i++] = { k, j };
  return result;
}