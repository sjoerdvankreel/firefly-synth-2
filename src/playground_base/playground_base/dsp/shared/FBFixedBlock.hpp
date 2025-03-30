#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

// TODO rename to block
template <class T>
class alignas(FBFixedBlockAlign) FBFixedArray 
{
  std::array<T, FBFixedBlockSamples> _data = {};
public:
  void Fill(T val) { _data.fill(val); }
  T& operator[](int i) { return _data[i]; }
  T const& operator[](int i) const { return _data[i]; }
  std::array<T, FBFixedBlockSamples>& Data() { return _data; }
  std::array<T, FBFixedBlockSamples> const& Data() const { return _data; }
};

template <class T>
struct alignas(FBFixedBlockAlign) FBFixedAudioArray
{ 
  std::array<FBFixedArray<T>, 2> _data = {};
public:
  FBFixedArray<T>& operator[](int c) { return _data[c]; }
  FBFixedArray<T> const& operator[](int c) const { return _data[c]; }
  void Fill(T val) { for(int c = 0; c < 2; c++) _data[c].fill(val); }
};

typedef FBFixedArray<float> FBFixedFloatArray;
typedef FBFixedArray<double> FBFixedDoubleArray;
typedef FBFixedAudioArray<float> FBFixedFloatAudioArray;
typedef FBFixedAudioArray<double> FBFixedDoubleAudioArray;