#pragma once

enum class FBParamType { Boolean, Discrete, List, Note, TimeSig, Linear, FreqOct };

bool
FBParamTypeIsList(FBParamType type);
bool
FBParamTypeIsStepped(FBParamType type);