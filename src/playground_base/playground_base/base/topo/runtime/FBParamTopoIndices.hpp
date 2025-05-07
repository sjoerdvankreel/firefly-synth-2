#pragma once

#include <playground_base/base/topo/runtime/FBTopoIndices.hpp>

struct FBParamTopoIndices final
{
  FBTopoIndices module = {};
  FBTopoIndices param = {};
  auto operator<=>(FBParamTopoIndices const&) const = default;
};