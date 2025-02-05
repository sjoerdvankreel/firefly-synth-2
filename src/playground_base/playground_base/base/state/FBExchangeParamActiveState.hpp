#pragma once

#include <compare>

struct FBExchangeParamActiveState final
{
  bool active = {};
  float minValue = {};
  float maxValue = {};
  bool operator==(FBExchangeParamActiveState const&) const = default;
};