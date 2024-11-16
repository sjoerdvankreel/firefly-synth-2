#pragma once

#include <string>
#include <vector>
#include <memory>

struct FBPluginStaticTopology;
struct FBPluginRuntimeTopology;

std::unique_ptr<FBPluginRuntimeTopology> 
FBGenerateRuntimeTopology(
  FBPluginStaticTopology const& staticTopology);

struct FBPluginStaticParameterBase
{
  virtual ~FBPluginStaticParameterBase() = default;

  int index; // static index, discrete and continuous start at 0
  int slotCount; // multi-slot params are useful for mod matrices
  std::string name;
  std::string uniqueId;
};

struct FBPluginStaticDiscreteParameter :
FBPluginStaticParameterBase
{
  int stepCount;
};

struct FBPluginStaticContinuousParameter :
FBPluginStaticParameterBase
{
};

struct FBPluginStaticModule
{
  int index; // static index
  int slotCount;
  std::string name;
  std::string uniqueId;
  std::vector<FBPluginStaticDiscreteParameter> discreteParameters;
  std::vector<FBPluginStaticContinuousParameter> continuousParameters;
};

struct FBPluginStaticTopology
{
  std::vector<FBPluginStaticModule> modules;
};

struct FBPluginRuntimeParameter
{
  int index; // runtime index, discrete first, continuous next
  int stepCount; // or 0 for continuous
  std::string name;
  std::string uniqueId;
  std::shared_ptr<FBPluginStaticParameterBase> staticTopology;
};

struct FBPluginRuntimeModule
{
  int index; // runtime index
  std::string name;
  std::string uniqueId;
  std::vector<FBPluginRuntimeParameter> parameters;
  std::shared_ptr<FBPluginStaticModule> staticTopology;
};

struct FBPluginRuntimeTopology
{
  std::vector<FBPluginRuntimeModule> modules;
};