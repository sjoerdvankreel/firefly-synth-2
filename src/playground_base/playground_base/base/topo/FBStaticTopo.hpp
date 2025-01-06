#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBPlugVersion.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBSpecialParams.hpp>

#include <vector>
#include <memory>
#include <functional>

struct FBStaticTopo;
typedef std::function<FBSpecialParams (
  FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;

class FBPlugGUI;
struct FBRuntimeTopo;
class IFBHostGUIContext;
typedef std::function<std::unique_ptr<FBPlugGUI>(
  FBRuntimeTopo const* topo, IFBHostGUIContext*)>
FBPlugGUIFactory;

struct FBStaticTopo final
{
  FBPlugVersion version = {};
  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);

  FBPlugGUIFactory guiFactory = {};
  void* (*allocRawProcState)() = {};
  void* (*allocRawScalarState)() = {};
  void (*freeRawProcState)(void* state) = {};
  void (*freeRawScalarState)(void* state) = {};
  FBSpecialParamsSelector specialSelector = {};
};