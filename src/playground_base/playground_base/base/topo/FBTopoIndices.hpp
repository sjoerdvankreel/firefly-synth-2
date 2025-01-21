#pragma once

struct FBTopoIndices final
{
  int index = {};
  int slot = {};
  auto operator<=>(FBTopoIndices const&) const = default;
};