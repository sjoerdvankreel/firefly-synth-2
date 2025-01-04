#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBParamType.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>
#include <playground_base/base/topo/param/FBParamAddrSelector.hpp>

#include <string>
#include <vector>
#include <optional>
#include <algorithm>

struct FBStaticParam final
{
public:
  bool acc = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string defaultText = {};

  FBParamType type = {};
  FBListParam list = {};
  FBBoolParam boolean = {};
  FBLinearParam linear = {};
  FBDiscreteParam discrete = {};

  FBScalarAddrSelector scalarAddr = {};
  FBVoiceAccAddrSelector voiceAccAddr = {};
  FBGlobalAccAddrSelector globalAccAddr = {};
  FBVoiceBlockAddrSelector voiceBlockAddr = {};
  FBGlobalBlockAddrSelector globalBlockAddr = {};    

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
};