#pragma once

enum class FBParamType { Boolean, Discrete, DiscreteLog2, List, Note, Bars, Linear, Log2 };

bool
FBParamTypeIsList(FBParamType type);
bool
FBParamTypeIsStepped(FBParamType type);