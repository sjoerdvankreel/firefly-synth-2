#pragma once

#include <functional>

typedef std::function<double* (
  int moduleSlot, int paramSlot, void* state)>
FBScalarParamAddrSelector;