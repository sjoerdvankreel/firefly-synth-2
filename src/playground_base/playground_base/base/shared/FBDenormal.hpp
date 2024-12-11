#pragma once

#include <utility>
#include <cstdint>

typedef std::pair<std::uint32_t, std::uint32_t> 
FBDenormalState;

FBDenormalState FBDisableDenormal();
void FBRestoreDenormal(FBDenormalState state);