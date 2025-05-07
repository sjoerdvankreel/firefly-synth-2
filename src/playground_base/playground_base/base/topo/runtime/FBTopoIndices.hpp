#pragma once

#include <compare>

struct FBTopoIndices final
{
  int index = {};
  int slot = {};
  auto operator<=>(FBTopoIndices const&) const = default;
};

struct FBParamTopoIndices final
{
  FBTopoIndices module = {};
  FBTopoIndices param = {};
  auto operator<=>(FBParamTopoIndices const&) const = default;
};