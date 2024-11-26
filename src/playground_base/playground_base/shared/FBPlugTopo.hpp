#pragma once

#include <playground_base/shared/FBSharedUtility.hpp>

#include <map>
#include <string>
#include <vector>
#include <functional>

#define FB_SINGLE_SLOT_COUNT 1

class FBFixedCVBlock;

struct FBScalarParamMemoryBase
{
  // dynamic_cast just to be sure
  virtual ~FBScalarParamMemoryBase() {}

  // interior pointers to derived
  std::vector<float*> accAddr;
  std::vector<float*> blockAddr;
};

struct FBProcessorParamMemoryBase
{
  // dynamic_cast just to be sure
  virtual ~FBProcessorParamMemoryBase() {}

  // interior pointers to derived
  std::vector<int*> posAddr;
  std::vector<FBFixedCVBlock*> denseAddr;
};

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

  std::function<float* (
    int moduleSlot, int paramSlot, 
    FBScalarParamMemoryBase& mem)> scalarAddr;
  std::function<int* (
    int moduleSlot, int paramSlot, 
    FBProcessorParamMemoryBase& mem)> posAddr;
  std::function<FBFixedCVBlock* (
    int moduleSlot, int paramSlot,
    FBProcessorParamMemoryBase& mem)> denseAddr;

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

  void InitScalarAddr(FBScalarParamMemoryBase& mem) const;
  void InitProcessorAddr(FBProcessorParamMemoryBase& mem) const;
};