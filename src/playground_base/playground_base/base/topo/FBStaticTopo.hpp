#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBPlugVersion.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBStaticStateTopo.hpp>

#include <vector>
#include <memory>
#include <functional>

class IFBPlugGUI;
struct FBRuntimeTopo;
class IFBHostGUIContext;
typedef std::function<std::unique_ptr<IFBPlugGUI>(
  FBRuntimeTopo const* topo, IFBHostGUIContext*)>
FBPlugGUIFactory;

struct FBStaticTopo final
{
  FBPlugVersion version = {};
  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);

  FBStaticStateTopo state = {};
  FBPlugGUIFactory guiFactory = {};
};