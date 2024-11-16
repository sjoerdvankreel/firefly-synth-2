#pragma once

#include <array>
#include <cstddef>

template <int Count>
using FBSingleBlock = std::array<float, Count>;
template <int Count>
using FBDualBlock = std::array<FBSingleBlock<Count>, 2>;

template <int Count>
inline int constexpr FBBlockSize(FBSingleBlock<Count> const& block)
{
  return static_cast<int>(block.size());
}

template <int Count>
inline int constexpr FBBlockSize(FBDualBlock<Count> const& block)
{
  return FBBlockSize(block[0]);
}