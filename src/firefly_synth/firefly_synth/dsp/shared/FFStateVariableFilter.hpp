#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <cmath>
#include <numbers>

inline float constexpr FFMaxStateVariableFilterRes = 0.99f;
inline float constexpr FFMinStateVariableFilterFreq = 20.0f;
inline float constexpr FFMaxStateVariableFilterFreq = 20000.0f;

enum class FFStateVariableFilterMode
{ LPF, BPF, HPF, BSF, APF, PEQ, BLL, LSH, HSH };

// https://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
template <int Channels>
class alignas(FBSIMDAlign) FFStateVariableFilter final
{
  double _m0 = {};
  double _m1 = {};
  double _m2 = {};
  double _a1 = {};
  double _a2 = {};
  double _a3 = {};
  alignas(FBSIMDAlign) std::array<double, Channels> _ic1eq = {};
  alignas(FBSIMDAlign) std::array<double, Channels> _ic2eq = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFStateVariableFilter);
  void Reset();
  double Next(
    int channel, double in);
  void Set(
    FFStateVariableFilterMode mode, double sampleRate,
    double freqHz, double resNorm, double gainDb);
};

template <int Channels>
inline void
FFStateVariableFilter<Channels>::Reset()
{
  _m0 = {};
  _m1 = {};
  _m2 = {};
  _a1 = {};
  _a2 = {};
  _a3 = {};
  _ic1eq.fill(0.0);
  _ic2eq.fill(0.0);
}

template <int Channels>
inline double 
FFStateVariableFilter<Channels>::Next(
  int channel, double in)
{
  FB_ASSERT(!std::isnan(in));
  FB_ASSERT(!std::isinf(in));
  double v0 = in;
  double v3 = v0 - this->_ic2eq[channel];
  double v1 = this->_a1 * this->_ic1eq[channel] + this->_a2 * v3;
  double v2 = this->_ic2eq[channel] + this->_a2 * this->_ic1eq[channel] + this->_a3 * v3;
  this->_ic1eq[channel] = 2 * v1 - this->_ic1eq[channel];
  this->_ic2eq[channel] = 2 * v2 - this->_ic2eq[channel];
  double result = this->_m0 * v0 + this->_m1 * v1 + this->_m2 * v2;
  FB_ASSERT(!std::isnan(result));
  FB_ASSERT(!std::isinf(result));
  FB_ASSERT(!std::isnan(static_cast<float>(result)));
  FB_ASSERT(!std::isinf(static_cast<float>(result)));
  return result;
}

template <int Channels>
inline void 
FFStateVariableFilter<Channels>::Set(
  FFStateVariableFilterMode mode, double sampleRate,
  double freqHz, double resNorm, double gainDb)
{
  // check for graphs
#ifndef NDEBUG
  double nyquist = sampleRate * 0.5f;
  double minFilterFreq = FFMinStateVariableFilterFreq;
  double maxFilterFreq = FFMaxStateVariableFilterFreq;
  if (FFMaxStateVariableFilterFreq > nyquist)
  {
    minFilterFreq *= nyquist / FFMaxStateVariableFilterFreq;
    maxFilterFreq *= nyquist / FFMaxStateVariableFilterFreq;
  }
  FB_ASSERT(minFilterFreq - 0.1 <= freqHz && freqHz <= maxFilterFreq + 0.1);
#endif

  double a = 0.0;
  double k = 2.0 - 2.0 * resNorm * FFMaxStateVariableFilterRes;
  double g = std::tan(std::numbers::pi * freqHz / sampleRate);

  if (mode >= FFStateVariableFilterMode::BLL)
  {
    a = std::pow(10.0, gainDb / 40.0);
    switch (mode)
    {
    case FFStateVariableFilterMode::BLL: k = k / a; break;
    case FFStateVariableFilterMode::LSH: g = g / std::sqrt(a); break;
    case FFStateVariableFilterMode::HSH: g = g * std::sqrt(a); break;
    default: FB_ASSERT(false); break;
    }
  }

  this->_a1 = 1.0 / (1.0 + g * (g + k));
  this->_a2 = g * this->_a1;
  this->_a3 = g * this->_a2;

  this->_m0 = 0.0f;
  this->_m1 = 0.0f;
  this->_m2 = 0.0f;
  switch (mode)
  {
  case FFStateVariableFilterMode::LPF: this->_m2 = 1.0; break;
  case FFStateVariableFilterMode::BPF: this->_m1 = 1.0; break;
  case FFStateVariableFilterMode::HPF: this->_m0 = 1.0; this->_m1 = -k; this->_m2 = -1.0; break;
  case FFStateVariableFilterMode::BSF: this->_m0 = 1.0; this->_m1 = -k; break;
  case FFStateVariableFilterMode::PEQ: this->_m0 = 1.0; this->_m1 = -k; this->_m2 = -2.0; break;
  case FFStateVariableFilterMode::APF: this->_m0 = 1.0; this->_m1 = -2.0 * k; break;
  case FFStateVariableFilterMode::BLL: this->_m0 = 1.0; this->_m1 = k * (a * a - 1.0); break;
  case FFStateVariableFilterMode::LSH: this->_m0 = 1.0; this->_m1 = k * (a - 1.0); this->_m2 = a * a - 1.0; break;
  case FFStateVariableFilterMode::HSH: this->_m0 = a * a; this->_m1 = k * (1.0 - a * a); this->_m2 = 1.0 - a * a; break;
  default: FB_ASSERT(false);  break;
  }
}