#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <map>
#include <string>
#include <vector>
#include <functional>

class FBFixedCVBlock;
struct FBProcParamState;
struct FBProcStateAddrs;
struct FBScalarStateAddrs;

struct FBStaticParam
{
  int slotCount = {};
  int valueCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
  std::function<float* (int moduleSlot, int paramSlot,
    FBScalarStateAddrs& addrs)> scalarAddr;
  std::function<FBProcParamState* (int moduleSlot, int paramSlot,
    FBProcStateAddrs& addrs)> procAddr;
};

struct FBStaticModule
{
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::vector<FBStaticParam> acc = {};
  std::vector<FBStaticParam> block = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};

struct FBStaticTopo
{
  std::vector<FBStaticModule> modules = {};
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

  void InitProcAddrs(FBProcStateAddrs& addrs) const;
  void InitScalarAddrs(FBScalarStateAddrs& addrs) const;
};