#pragma once

#include <functional>

typedef std::function<float* (
  int moduleSlot, int paramSlot, void* state)>
FBScalarParamAddrSelector;