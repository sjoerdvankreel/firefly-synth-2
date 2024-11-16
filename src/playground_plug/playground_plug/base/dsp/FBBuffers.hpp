#pragma once
#include <array>
#include <cstddef>

template <std::size_t Count>
using FBMonoBuffer = std::array<float, Count>;
template <std::size_t Count>
using FBStereoBuffer = std::array<FBMonoBuffer<Count>, 2>;