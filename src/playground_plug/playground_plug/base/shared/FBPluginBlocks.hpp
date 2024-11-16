#pragma once

#include <array>
#include <cstddef>

template <int Count>
using FBCVBlock = std::array<float, Count>;
template <int Count>
using FBAudioBlock = std::array<std::array<float, Count>, 2>;

template <int Count>
inline int constexpr FBBlockSize(FBCVBlock<Count> const& block)
{
  return static_cast<int>(block.size());
}

template <int Count>
inline int constexpr FBBlockSize(FBAudioBlock<Count> const& block)
{
  return static_cast<int>(block[0].size());
}