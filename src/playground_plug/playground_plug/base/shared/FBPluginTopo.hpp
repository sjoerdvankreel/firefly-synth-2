#pragma once

#include <map>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

int
FBMakeHash(std::string const& id);
std::string
FBMakeId(std::string id, int slot);
std::string
FBMakeName(std::string name, int slotCount, int slot);

inline double 
FBStepToNormalized(int stepCount, int step)
{
  return step / (double)stepCount;
}

inline int
FBNormalizedToStep(int stepCount, double normalized)
{
  return std::min(stepCount, (int)(normalized * (stepCount + 1)));
}

// all this stuff is templated for the sole reason to allow 
// getting to the actual memory (FFPluginBlock) without void* 's

template <class PluginBlock>
struct FBStaticParam
{
  int stepCount; // or 0 for continuous [0, 1]
  int slotCount; // multi-slot params are useful for mod matrices
  std::string id;
  std::string name;
  std::string unit;

  // reason for template
  float* (*plugParamAddr)(
    int moduleSlot, int paramSlot, PluginBlock* block);
  std::array<float, PluginBlock::BlockSize>* (*autoParamAddr)(
    int moduleSlot, int paramSlot, PluginBlock* block);
};

template <class PluginBlock>
struct FBStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FBStaticParam<PluginBlock>> plugParams; // editable by UI only, backed by scalar
  std::vector<FBStaticParam<PluginBlock>> autoParams; // automatable/modulatable, backed by dense buffer
};

template <class PluginBlock>
struct FBStaticTopo
{
  std::vector<FBStaticModule<PluginBlock>> modules;
};

template <class PluginBlock>
struct FBRuntimeParam
{
  int tag; // VST3 / CLAP param tag
  int moduleSlot;
  int paramSlot;

  std::string id;
  std::string name;
  FBStaticParam<PluginBlock> staticTopo;

  FBRuntimeParam(
    FBStaticModule<PluginBlock> const& module, int moduleSlot,
    FBStaticParam<PluginBlock> const& param, int paramSlot);
};

template <class PluginBlock>
struct FBRuntimeModule
{
  std::string id;
  std::string name;
  std::vector<FBRuntimeParam<PluginBlock>> plugParams;
  std::vector<FBRuntimeParam<PluginBlock>> autoParams;

  FBRuntimeModule(
    FBStaticModule<PluginBlock> const& module, int slot);
};

template <class PluginBlock>
struct FBRuntimeTopo
{
  std::map<int, int> tagToPlugParam;
  std::map<int, int> tagToAutoParam;
  std::vector<FBRuntimeModule<PluginBlock>> modules;
  std::vector<FBRuntimeParam<PluginBlock>> plugParams;
  std::vector<FBRuntimeParam<PluginBlock>> autoParams;

  FBRuntimeTopo(FBStaticTopo<PluginBlock> const& topo);
};

template <class PluginBlock>
FBRuntimeParam<PluginBlock>::
FBRuntimeParam(
  FBStaticModule<PluginBlock> const& module, int moduleSlot,
  FBStaticParam<PluginBlock> const& param, int paramSlot)
{
  moduleSlot = moduleSlot;
  paramSlot = paramSlot;
  staticTopo = param;

  id = FBMakeId(module.id, moduleSlot);
  id += "-" + FBMakeId(param.id, paramSlot);
  name = FBMakeName(module.name, module.slotCount, moduleSlot);
  name += " " + FBMakeName(param.name, param.slotCount, paramSlot);
  tag = FBMakeHash(id);
}

template <class PluginBlock>
FBRuntimeModule<PluginBlock>::
FBRuntimeModule(
  FBStaticModule<PluginBlock> const& module, int slot)
{
  id = FBMakeId(module.id, slot);
  name = FBMakeName(module.name, module.slotCount, slot);
  for (int ppi = 0; ppi < module.plugParams.size(); ppi++)
    for (int pps = 0; pps < module.plugParams[ppi].slotCount; pps++)
      plugParams.push_back(FBRuntimeParam(module, slot, module.plugParams[ppi], pps));
  for (int api = 0; api < module.autoParams.size(); api++)
    for (int aps = 0; aps < module.autoParams[api].slotCount; aps++)
      autoParams.push_back(FBRuntimeParam(module, slot, module.autoParams[api], aps));
}

template <class PluginBlock>
FBRuntimeTopo<PluginBlock>::
FBRuntimeTopo(
  FBStaticTopo<PluginBlock> const& topo)
{
  for (int mi = 0; mi < topo.modules.size(); mi++)
    for (int ms = 0; ms < topo.modules[mi].slotCount; ms++)
      modules.push_back(FBRuntimeModule(topo.modules[mi], ms));
  for (int m = 0; m < modules.size(); m++)
    for (int pp = 0; pp < modules[m].plugParams.size(); pp++)
      plugParams.push_back(modules[m].plugParams[pp]);
  for (int m = 0; m < modules.size(); m++)
    for (int ap = 0; ap < modules[m].autoParams.size(); ap++)
      autoParams.push_back(modules[m].autoParams[ap]);
  for (int pp = 0; pp < plugParams.size(); pp++)
    tagToPlugParam[plugParams[pp].tag] = pp;
  for (int ap = 0; ap < autoParams.size(); ap++)
    tagToAutoParam[autoParams[ap].tag] = ap;
}