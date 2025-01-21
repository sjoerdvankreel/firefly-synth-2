#pragma once

struct FBTopoIndices final
{
  int slot = {};
  int index = {};
  auto operator<=>(FBTopoIndices const&) const = default;
};