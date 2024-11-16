#pragma once
#include <array>
#include <cstddef>

template <std::size_t Count>
using FBCVBlock = std::array<float, Count>;
template <std::size_t Count>
using FBAudioBlock = std::array<std::array<float, Count>, 2>;

template <std::size_t Count>
inline std::size_t constexpr FBBlockSize(FBCVBlock<Count> const& block)
{
  return block.size();
}

template <std::size_t Count>
inline std::size_t constexpr FBBlockSize(FBAudioBlock<Count> const& block)
{
  return block[0].size();
}