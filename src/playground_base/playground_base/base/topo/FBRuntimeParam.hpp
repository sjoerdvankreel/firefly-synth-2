#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <string>

struct FBStaticModule;

struct FBRuntimeParam final
{
  int const moduleIndex;
  int const moduleSlot;
  int const paramSlot;
  FBStaticParam const static_;

  std::string const longName;
  std::string const shortName;
  std::string const id;
  int const tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& module, int moduleIndex, int moduleSlot,
    FBStaticParam const& param, int paramSlot);
};