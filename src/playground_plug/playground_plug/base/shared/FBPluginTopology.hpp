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
  int index; // static index - discrete and continuous both start at 0
  int slotCount; // multi-slot params are useful for mod matrices
  std::string name;
  std::string uniqueId;
};

struct FBPluginStaticDiscreteParameter:
public FBPluginStaticParameterBase
{
  int stepCount;
};

struct FBPluginStaticContinuousParameter :
public FBPluginStaticParameterBase
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

struct FBPluginRuntimeParameterBase
{
  int index; // runtime index - discrete first, continous next
  std::string name;
  std::string uniqueId;
};

struct FBPluginRuntimeDiscreteParameter:
public FBPluginRuntimeParameterBase
{
  std::shared_ptr<FBPluginStaticDiscreteParameter> staticTopology;
};

struct FBPluginRuntimeContinousParameter:
public FBPluginRuntimeParameterBase
{
  std::shared_ptr<FBPluginStaticContinuousParameter> staticTopology;
};

struct FBPluginRuntimeModule
{
  int index; // runtime index
  std::string name;
  std::string uniqueId;
  std::shared_ptr<FBPluginStaticModule> staticTopology;
  std::vector<FBPluginRuntimeDiscreteParameter> discreteParameters;
  std::vector<FBPluginRuntimeContinousParameter> continuousParameters;
};

struct FBPluginRuntimeTopology
{
  std::vector<FBPluginRuntimeModule> modules;
};