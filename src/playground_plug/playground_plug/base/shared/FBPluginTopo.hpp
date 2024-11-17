#pragma once

#include <string>
#include <vector>
#include <memory>

struct FBRuntimeModule;

struct FBStaticParam
{
  int stepCount; // or 0 for continuous [0, 1]
  int slotCount; // multi-slot params are useful for mod matrices
  std::string id;
  std::string name;
  std::string unit;
};

struct FBStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FBStaticParam> plugParams; // editable by UI only, backed by scalar
  std::vector<FBStaticParam> autoParams; // automatable/modulatable, backed by dense buffer
};

struct FBStaticTopo
{
  std::vector<FBStaticModule> modules;
};

struct FBRuntimeParam
{
  int tag; // VST3 / CLAP param tag
  std::string id;
  std::string name;
  FBStaticParam staticTopo;

  FBRuntimeParam(
    FBStaticModule const& module, int moduleSlot,
    FBStaticParam const& param, int paramSlot);
};

struct FBRuntimeModule
{
  std::string id;
  std::string name;
  std::vector<FBRuntimeParam> plugParams;
  std::vector<FBRuntimeParam> autoParams;

  FBRuntimeModule(
    FBStaticModule const& module, int slot);
};

struct FBRuntimeTopo
{
  std::vector<FBRuntimeModule> modules;
  FBRuntimeTopo(FBStaticTopo const& topo);
};