#pragma once

#include <array>

template <class T, int D1>
using FBMDArray1 = std::array<T, D1>;
template <class T, int D1, int D2>
using FBMDArray2 = std::array<FBMDArray1<T, D1>, D2>;
template <class T, int D1, int D2, int D3>
using FBMDArray3 = std::array<FBMDArray2<T, D1, D2>, D3>;
template <class T, int D1, int D2, int D3, int D4>
using FBMDArray4 = std::array<FBMDArray3<T, D1, D2, D3>, D4>;