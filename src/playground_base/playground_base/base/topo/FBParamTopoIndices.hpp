#pragma once

#include <playground_base/base/topo/FBTopoIndices.hpp>

struct FBParamTopoIndices final
{
  FBTopoIndices param = {};
  FBTopoIndices module = {};
  auto operator<=>(FBParamTopoIndices const&) const = default;
};