#pragma once

#include <playground_plug/base/FFBaseUtility.hpp>

#include <map>
#include <string>
#include <vector>

inline double 
FFDiscreteToNormalized(int count, int index)
{ return index / (count - 1.0); }

inline int
FFNormalizedToDiscrete(int count, double normalized)
{ return std::min(count - 1, (int)(normalized * count)); }

inline bool
FFNormalizedToBool(double normalized)
{ return FFNormalizedToDiscrete(2, normalized) != 0; }

struct FFStaticParam
{
  int slotCount;
  int valueCount;
  std::string id;
  std::string name;
  std::string unit;

  FF_EXPLICIT_COPY_MOVE_DEFCTOR(FFStaticParam);
};

struct FFStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FFStaticParam> accParams;
  std::vector<FFStaticParam> blockParams;

  FF_EXPLICIT_COPY_MOVE_DEFCTOR(FFStaticModule);
};

struct FFStaticTopo
{
  std::vector<FFStaticModule> modules;

  FF_EXPLICIT_COPY_MOVE_DEFCTOR(FFStaticTopo);
};

struct FFRuntimeParam
{
  int const moduleSlot;
  int const paramSlot;
  FFStaticParam const staticParam;

  std::string const longName;
  std::string const shortName;
  std::string const id;
  int const tag;

  FF_EXPLICIT_COPY_MOVE_NODEFCTOR(FFRuntimeParam);
  FFRuntimeParam(
    FFStaticModule const& staticModule, int moduleSlot,
    FFStaticParam const& staticParam, int paramSlot);
};

struct FFRuntimeModule
{
  std::string const name;
  std::vector<FFRuntimeParam> const accParams;
  std::vector<FFRuntimeParam> const blockParams;

  FF_EXPLICIT_COPY_MOVE_NODEFCTOR(FFRuntimeModule);
  FFRuntimeModule(
    FFStaticModule const& staticModule, int moduleSlot);
};

struct FFRuntimeTopo
{
  FF_EXPLICIT_COPY_MOVE_NODEFCTOR(FFRuntimeTopo);
  FFRuntimeTopo(FFStaticTopo const& staticTopo);

  std::map<int, int> const tagToAccParam;
  std::map<int, int> const tagToBlockParam;
  std::vector<FFRuntimeModule> const modules;
  std::vector<FFRuntimeParam> const accParams;
  std::vector<FFRuntimeParam> const blockParams;
};