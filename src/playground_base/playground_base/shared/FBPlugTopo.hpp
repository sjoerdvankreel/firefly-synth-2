#pragma once

#include <playground_base/shared/FBSharedUtility.hpp>

#include <map>
#include <string>
#include <vector>

inline double 
FBDiscreteToNormalized(int count, int index)
{ return index / (count - 1.0); }

inline int
FBNormalizedToDiscrete(int count, double normalized)
{ return std::min(count - 1, (int)(normalized * count)); }

inline bool
FBNormalizedToBool(double normalized)
{ return FBNormalizedToDiscrete(2, normalized) != 0; }

struct FBStaticParam
{
  int slotCount;
  int valueCount;
  std::string id;
  std::string name;
  std::string unit;

  FF_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
};

struct FBStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FBStaticParam> accParams;
  std::vector<FBStaticParam> blockParams;

  FF_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};

struct FBStaticTopo
{
  std::vector<FBStaticModule> modules;
  FF_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
};

struct FBRuntimeParam
{
  int const moduleSlot;
  int const paramSlot;
  FBStaticParam const staticParam;

  std::string const longName;
  std::string const shortName;
  std::string const id;
  int const tag;

  FF_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& staticModule, int moduleSlot,
    FBStaticParam const& staticParam, int paramSlot);
};

struct FFRuntimeModule
{
  std::string const name;
  std::vector<FBRuntimeParam> const accParams;
  std::vector<FBRuntimeParam> const blockParams;

  FF_EXPLICIT_COPY_MOVE_NODEFCTOR(FFRuntimeModule);
  FFRuntimeModule(
    FBStaticModule const& staticModule, int moduleSlot);
};

struct FFRuntimeTopo
{
  FF_EXPLICIT_COPY_MOVE_NODEFCTOR(FFRuntimeTopo);
  FFRuntimeTopo(FBStaticTopo const& staticTopo);

  std::vector<FFRuntimeModule> const modules;
  std::vector<FBRuntimeParam> const accParams;
  std::vector<FBRuntimeParam> const blockParams;
  std::map<int, int> const tagToAccParam;
  std::map<int, int> const tagToBlockParam;
};