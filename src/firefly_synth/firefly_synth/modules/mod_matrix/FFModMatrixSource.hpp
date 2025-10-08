#pragma once

#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

struct FFModMatrixSource
{
  // mind order
  bool isGlobal = {};
  bool onNote = {};
  FBCVOutputTopoIndices indices = {};
};