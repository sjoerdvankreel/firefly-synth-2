#pragma once

enum class FBParamType { Boolean, Discrete, List, Note, Bars, Linear, FreqOct };

bool
FBParamTypeIsList(FBParamType type);
bool
FBParamTypeIsStepped(FBParamType type);