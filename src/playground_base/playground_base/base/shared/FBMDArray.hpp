#pragma once

#include <array>

template <class T, int D1>
using FBMDArray1 = std::array<T, D1>;
template <class T, int D1, int D2>
using FBMDArray2 = std::array<FBMDArray1<T, D1>, D2>;