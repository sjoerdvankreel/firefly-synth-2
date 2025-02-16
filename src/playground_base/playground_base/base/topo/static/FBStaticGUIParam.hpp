#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBStaticParamBase.hpp>
#include <playground_base/base/topo/static/FBScalarParamAddrs.hpp>

struct FBStaticGUIParam final:
public FBStaticParamBase
{
public:
  FBScalarParamAddrSelector addrSelector = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticGUIParam);
};