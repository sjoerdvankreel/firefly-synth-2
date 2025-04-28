#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

// TODO drop
template <class T>
class alignas(FBSIMDAlign) FBFixedArray
{
  std::array<T, FBFixedBlockSamples> _data = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedArray);

  T& operator[](int i) { return _data[i]; }
  T const& operator[](int i) const { return _data[i]; }
  std::array<T, FBFixedBlockSamples>& Data() { return _data; }
  std::array<T, FBFixedBlockSamples> const& Data() const { return _data; }

  void Fill(T val) { _data.fill(val); }
  T const& Last() { return _data[FBFixedBlockSamples - 1]; }
  void Add(FBFixedArray<T> const& rhs) { for (int s = 0; s < FBFixedBlockSamples; s++) _data[s] += rhs[s]; }
  void Mul(FBFixedArray<T> const& rhs) { for (int s = 0; s < FBFixedBlockSamples; s++) _data[s] *= rhs[s]; }
  void CopyTo(FBFixedArray<T>& rhs) const { for (int s = 0; s < FBFixedBlockSamples; s++) rhs[s] = _data[s]; }
  void Add(FBSIMDArray<float, FBFixedBlockSamples> const& rhs) { for (int s = 0; s < FBFixedBlockSamples; s++) _data[s] += rhs.Get(s); }
};

template <class T>
struct alignas(FBSIMDAlign) FBFixedAudioArray
{ 
  std::array<FBFixedArray<T>, 2> _data = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedAudioArray);
  FBFixedArray<T>& operator[](int c) { return _data[c]; }
  FBFixedArray<T> const& operator[](int c) const { return _data[c]; }

  void Fill(T val) { for(int c = 0; c < 2; c++) _data[c].Data().fill(val); }
  void Mul(FBFixedArray<T> const& rhs) { for (int c = 0; c < 2; c++) _data[c].Mul(rhs); }
  void Add(FBFixedAudioArray<T> const& rhs) { for (int c = 0; c < 2; c++) _data[c].Add(rhs[c]); }
  void Add(FBSIMDArray2<float, FBFixedBlockSamples, 2> const& rhs) { for (int c = 0; c < 2; c++) _data[c].Add(rhs[c]); }
  void CopyTo(FBFixedAudioArray<T>& rhs) const { for (int c = 0; c < 2; c++) _data[c].CopyTo(rhs[c]); }
};

typedef FBFixedArray<float> FBFixedFloatArray;
typedef FBFixedArray<double> FBFixedDoubleArray;
typedef FBFixedAudioArray<float> FBFixedFloatAudioArray;
typedef FBFixedAudioArray<double> FBFixedDoubleAudioArray;

inline void 
FBFixedFloatToDoubleArray(FBFixedFloatArray const& floats, FBFixedDoubleArray& doubles)
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    doubles[s] = floats[s];
}

inline void
FBFixedDoubleToFloatArray(FBFixedDoubleArray const& doubles, FBFixedFloatArray& floats)
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    floats[s] = static_cast<float>(doubles[s]);
}

inline void
FBFixedFloatAudioToDoubleArray(FBFixedFloatAudioArray const& floats, FBFixedDoubleAudioArray& doubles)
{
  for (int c = 0; c < 2; c++)
    FBFixedFloatToDoubleArray(floats[c], doubles[c]);
}

inline void
FBFixedDoubleAudioToFloatArray(FBFixedDoubleAudioArray const& doubles, FBFixedFloatAudioArray& floats)
{
  for (int c = 0; c < 2; c++)
    FBFixedDoubleToFloatArray(doubles[c], floats[c]);
}