#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <map>
#include <string>
#include <vector>
#include <functional>

struct FBProcAddrs;
struct FBScalarAddrs;
class FBFixedCVBlock;

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

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
  std::function<int* (int moduleSlot, int paramSlot, FBProcAddrs& addrs)> posAddr;
  std::function<float* (int moduleSlot, int paramSlot, FBScalarAddrs& addrs)> scalarAddr;
  std::function<FBFixedCVBlock* (int moduleSlot, int paramSlot, FBProcAddrs& addrs)> cvAddr;
};

struct FBStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FBStaticParam> acc;
  std::vector<FBStaticParam> block;
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
  FBStaticParam const static_;

  std::string const longName;
  std::string const shortName;
  std::string const id;
  int const tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& module, int moduleSlot,
    FBStaticParam const& param, int paramSlot);
};

struct FBRuntimeModule
{
  std::string const name;
  std::vector<FBRuntimeParam> const acc;
  std::vector<FBRuntimeParam> const block;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
  FBRuntimeModule(FBStaticModule const& module, int slot);
};

struct FBRuntimeTopo
{
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);
  FBRuntimeTopo(FBStaticTopo const& static_);

  std::vector<FBRuntimeModule> const modules;
  std::vector<FBRuntimeParam> const acc;
  std::vector<FBRuntimeParam> const block;
  std::map<int, int> const tagToAcc;
  std::map<int, int> const tagToBlock;

  void InitProcAddrs(FBProcAddrs& addrs) const;
  void InitScalarAddrs(FBScalarAddrs& addrs) const;
};