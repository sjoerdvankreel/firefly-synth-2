#pragma once

#include <string>
#include <vector>
#include <memory>

struct FBPluginStaticTopology;
struct FBPluginRuntimeTopology;

std::unique_ptr<FBPluginRuntimeTopology> 
FBGenerateRuntimeTopology(
  FBPluginStaticTopology const& staticTopology);

struct FBPluginStaticParameter
{
  int slotCount; // multi-slot params are useful for mod matrices
  int stepCount; // or 0 for continuous
  std::string name;
  std::string uniqueId;
};

struct FBPluginStaticModule
{
  int slotCount;
  std::string name;
  std::string uniqueId;
  std::vector<FBPluginStaticParameter> parameters;
};

struct FBPluginStaticTopology
{
  std::vector<FBPluginStaticModule> modules;
};

struct FBPluginRuntimeParameter
{
  int uniqueIdHash; // VST3 / CLAP param tag
  std::string name;
  std::string uniqueId;
  std::shared_ptr<FBPluginStaticParameter> staticTopology;
};

struct FBPluginRuntimeModule
{
  std::string name;
  std::string uniqueId;
  std::vector<FBPluginRuntimeParameter> parameters;
  std::shared_ptr<FBPluginStaticModule> staticTopology;
};

struct FBPluginRuntimeTopology
{
  std::vector<FBPluginRuntimeModule> modules;
};