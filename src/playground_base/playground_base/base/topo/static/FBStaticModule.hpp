#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBModuleAddrs.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/static/FBStaticGUIParam.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <functional>

class FBPlugGUI;
struct FBModuleGraphComponentData;

typedef std::function<void(FBModuleGraphComponentData* graphData)>
FBModuleGraphRenderer;

struct FBStaticModule final
{
  bool voice = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  FBModuleAddrSelectors addrSelectors = {};
  FBModuleGraphRenderer graphRenderer = {};
  std::vector<FBStaticParam> params = {};
  std::vector<FBStaticGUIParam> guiParams = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);

  template <class ParamIndex>
  int NormalizedToNoteFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  bool NormalizedToBoolFast(ParamIndex index, float normalized) const;
  template <class T, class ParamIndex>
  T NormalizedToListFast(ParamIndex index, float normalized) const;
};

template <class ParamIndex> 
inline bool
FBStaticModule::NormalizedToBoolFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Boolean().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline int
FBStaticModule::NormalizedToNoteFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Note().NormalizedToPlainFast(normalized);
}

template <class T, class ParamIndex>
inline T
FBStaticModule::NormalizedToListFast(ParamIndex index, float normalized) const
{
  return static_cast<T>(params[static_cast<int>(index)].List().NormalizedToPlainFast(normalized));
}