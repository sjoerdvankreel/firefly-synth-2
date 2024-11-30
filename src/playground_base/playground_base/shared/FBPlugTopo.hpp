#pragma once

#include <playground_base/base/shared/FBObjectLifetime.hpp>

#include <map>
#include <string>
#include <vector>
#include <functional>

class FBPlugCVBlock;

inline double 
FBDiscreteToNormalized(int count, int index)
{ return index / (count - 1.0); }

inline int
FBNormalizedToDiscrete(int count, double normalized)
{ return std::min(count - 1, (int)(normalized * count)); }

inline bool
FBNormalizedToBool(double normalized)
{ return FBNormalizedToDiscrete(2, normalized) != 0; }

struct FBScalarParamAddrsBase
{
  virtual ~FBScalarParamAddrsBase() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarParamAddrsBase);

  // interior pointers to derived
  std::vector<float*> acc;
  std::vector<float*> block;
};

struct FBDenseParamAddrsBase
{
  virtual ~FBDenseParamAddrsBase() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDenseParamAddrsBase);

  // interior pointers to derived
  std::vector<int*> pos;
  std::vector<FBPlugCVBlock*> cv;
};

struct FBStaticParam
{
  int slotCount;
  int valueCount;
  std::string id;
  std::string name;
  std::string unit;

  std::function<int* (
    int moduleSlot, int paramSlot, 
    FBDenseParamAddrsBase& addrs)> posAddr;
  std::function<FBPlugCVBlock* (
    int moduleSlot, int paramSlot,
    FBDenseParamAddrsBase& addrs)> denseAddr;
  std::function<float* (
    int moduleSlot, int paramSlot,
    FBScalarParamAddrsBase& addrs)> scalarAddr;

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
};

struct FBStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FBStaticParam> accParams;
  std::vector<FBStaticParam> blockParams;

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};

struct FBStaticTopo
{
  std::vector<FBStaticModule> modules;

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
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

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& staticModule, int moduleSlot,
    FBStaticParam const& staticParam, int paramSlot);
};

struct FBRuntimeModule
{
  std::string const name;
  std::vector<FBRuntimeParam> const accParams;
  std::vector<FBRuntimeParam> const blockParams;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
  FBRuntimeModule(
    FBStaticModule const& staticModule, int moduleSlot);
};

struct FBRuntimeTopo
{
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);
  FBRuntimeTopo(FBStaticTopo const& staticTopo);

  std::vector<FBRuntimeModule> const modules;
  std::vector<FBRuntimeParam> const accParams;
  std::vector<FBRuntimeParam> const blockParams;
  std::map<int, int> const tagToAccParam;
  std::map<int, int> const tagToBlockParam;

  void InitDenseAddrs(FBDenseParamAddrsBase& addrs) const;
  void InitScalarAddrs(FBScalarParamAddrsBase& addrs) const;
};