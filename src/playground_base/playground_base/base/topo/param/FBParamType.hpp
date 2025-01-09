#pragma once

enum class FBParamType { Boolean, Discrete, List, Linear, FreqOct };

bool
FBParamTypeIsStepped(FBParamType type);