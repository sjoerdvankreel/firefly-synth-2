#pragma once

enum class FBParamType { Boolean, Discrete, List, Note, Linear, FreqOct };

bool
FBParamTypeIsList(FBParamType type);
bool
FBParamTypeIsStepped(FBParamType type);