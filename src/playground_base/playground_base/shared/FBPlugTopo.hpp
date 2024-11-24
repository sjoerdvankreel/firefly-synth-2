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

struct FFStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FBStaticParam> accParams;
  std::vector<FBStaticParam> blockParams;

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
  FBStaticParam const staticParam;

  std::string const longName;
  std::string const shortName;
  std::string const id;
  int const tag;

  FF_EXPLICIT_COPY_MOVE_NODEFCTOR(FFRuntimeParam);
  FFRuntimeParam(
    FFStaticModule const& staticModule, int moduleSlot,
    FBStaticParam const& staticParam, int paramSlot);
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

  std::vector<FFRuntimeModule> const modules;
  std::vector<FFRuntimeParam> const accParams;
  std::vector<FFRuntimeParam> const blockParams;
  std::map<int, int> const tagToAccParam;
  std::map<int, int> const tagToBlockParam;
};