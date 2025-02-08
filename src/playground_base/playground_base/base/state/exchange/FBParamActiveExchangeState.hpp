#pragma once

#include <compare>

struct FBParamActiveExchangeState final
{
  bool active = {};
  float minValue = {};
  float maxValue = {};
  bool operator==(FBParamActiveExchangeState const&) const = default;
};