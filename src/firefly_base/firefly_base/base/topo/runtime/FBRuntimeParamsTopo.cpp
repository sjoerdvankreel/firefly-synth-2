#include <firefly_base/base/topo/runtime/FBRuntimeParamsTopo.hpp>

template <class TParam>
static std::unordered_map<int, int>
MakeParamTagToIndex(
  std::vector<TParam> const& params)
{
  std::unordered_map<int, int> result;
  for (int p = 0; p < params.size(); p++)
    result[params[p].tag] = p;
  return result;
}

template <class TParam>
static std::vector<std::vector<std::vector<std::vector<int>>>>
MakeParamTopoToRuntime(
  std::vector<TParam> const& params)
{
  std::vector<std::vector<std::vector<std::vector<int>>>> result = {};
  for (int p = 0; p < params.size(); p++)
  {
    int paramSlot = params[p].topoIndices.param.slot;
    int paramIndex = params[p].topoIndices.param.index;
    int moduleSlot = params[p].topoIndices.module.slot;
    int moduleIndex = params[p].topoIndices.module.index;
    if (result.size() <= moduleIndex)
      result.resize(moduleIndex + 1);
    if (result[moduleIndex].size() <= moduleSlot)
      result[moduleIndex].resize(moduleSlot + 1);
    if (result[moduleIndex][moduleSlot].size() <= paramIndex)
      result[moduleIndex][moduleSlot].resize(paramIndex + 1);
    if (result[moduleIndex][moduleSlot][paramIndex] <= paramSlot)
      result[moduleIndex][moduleSlot][paramIndex].resize(paramSlot + 1);
    result[moduleIndex][moduleSlot][paramIndex][paramSlot] = p;
  }
  return result;
}

static std::vector<FBRuntimeParam>
MakeRuntimeParams(
  std::vector<FBRuntimeModule> const& modules)
{
  std::vector<FBRuntimeParam> result;
  for (int m = 0; m < modules.size(); m++)
    for (int p = 0; p < modules[m].params.size(); p++)
      result.push_back(modules[m].params[p]);
  return result;
}

static std::vector<FBRuntimeGUIParam>
MakeRuntimeGUIParams(
  std::vector<FBRuntimeModule> const& modules)
{
  std::vector<FBRuntimeGUIParam> result;
  for (int m = 0; m < modules.size(); m++)
    for (int p = 0; p < modules[m].guiParams.size(); p++)
      result.push_back(modules[m].guiParams[p]);
  return result;
}

template <>
FBRuntimeParamsTopo<FBRuntimeParam>::
FBRuntimeParamsTopo(std::vector<FBRuntimeModule> const& modules):
params(MakeRuntimeParams(modules)),
paramTagToIndex(MakeParamTagToIndex(params)),
paramTopoToRuntime(MakeParamTopoToRuntime(params)) {}

template <>
FBRuntimeParamsTopo<FBRuntimeGUIParam>::
FBRuntimeParamsTopo(std::vector<FBRuntimeModule> const& modules) :
params(MakeRuntimeGUIParams(modules)),
paramTagToIndex(MakeParamTagToIndex(params)),
paramTopoToRuntime(MakeParamTopoToRuntime(params)) {}

template <class TParam>
TParam const*
FBRuntimeParamsTopo<TParam>::ParamAtTopo(
  FBParamTopoIndices const& topoIndices) const
{
  return &params[paramTopoToRuntime[topoIndices.module.index][topoIndices.module.slot][topoIndices.param.index][topoIndices.param.slot]];
}

template struct FBRuntimeParamsTopo<FBRuntimeParam>;
template struct FBRuntimeParamsTopo<FBRuntimeGUIParam>;
