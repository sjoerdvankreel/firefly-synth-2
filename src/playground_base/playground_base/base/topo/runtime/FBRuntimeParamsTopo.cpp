#include <playground_base/base/topo/runtime/FBRuntimeParamsTopo.hpp>

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
static std::map<FBParamTopoIndices, int>
MakeParamTopoToRuntime(
  std::vector<TParam> const& params)
{
  std::map<FBParamTopoIndices, int> result;
  for (int p = 0; p < params.size(); p++)
    result[params[p].topoIndices] = p;
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
    for (int p = 0; p < modules[m].params.size(); p++)
      result.push_back(modules[m].guiParams[p]);
  return result;
}

FBRuntimeParamsTopo<FBRuntimeParam>::
FBRuntimeParamsTopo(std::vector<FBRuntimeModule> const& modules):
params(MakeRuntimeParams(modules)),
tagToIndex(MakeParamTagToIndex(params)),
topoToRuntime(MakeParamTopoToRuntime(params)) {}

FBRuntimeParamsTopo<FBRuntimeGUIParam>::
FBRuntimeParamsTopo(std::vector<FBRuntimeModule> const& modules) :
params(MakeRuntimeGUIParams(modules)),
tagToIndex(MakeParamTagToIndex(params)),
topoToRuntime(MakeParamTopoToRuntime(params)) {}

template <class TParam>
TParam const*
FBRuntimeParamsTopo<TParam>::ParamAtTopo(
  FBParamTopoIndices const& topoIndices) const
{
  return &params[topoToRuntime.at(topoIndices)];
}