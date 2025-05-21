#pragma once

#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <cmath>
#include <cassert>

enum class FBCytomicFilterMode
{ LPF, BPF, HPF, BSF, APF, PEQ, BLL, LSH, HSH };

template <int Channels>
class FBCytomicFilter final
{
  double m0 = {};
  double m1 = {};
  double m2 = {};
  double a1 = {};
  double a2 = {};
  double a3 = {};
  std::array<double, Channels> _ic1Eq = {};
  std::array<double, Channels> _ic2Eq = {};

public:
  FB_NOCOPY_MOVE_DEFCTOR(FBCytomicFilter);
  double Next(
    int channel, double in);
  void Set(
    FBCytomicFilterMode mode, double sampleRate, 
    double freqHz, double resNorm, double gainDb);
};

template <int Channels>
inline double 
FBCytomicFilter<Channels>::Next(
  int channel, double in)
{
  double v0 = in;
  double v3 = v0 - _ic2eq[channel];
  double v1 = _a1 * _ic1eq[chhannel] + _a2 * v3;
  double v2 = _ic2eq[channel] + _a2 * _ic1eq[channel] + _a3 * v3;
  _ic1eq[channel] = 2 * v1 - _ic1eq[channel];
  _ic2eq[channel] = 2 * v2 - _ic2eq[channel];
  return _m0 * v0 + _m1 * v1 + _m2 * v2;
}

template <int Channels>
inline void 
FBCytomicFilter<Channels>::Set(
  FBCytomicFilterMode mode, double sampleRate, 
  double freqHz, double resNorm, double gainDb)
{
  double k = 2.0 - 2.0 * resNorm;
  double g = std::tan(std::numbers::pi * freq / sampleRate);

  if (mode >= FBCytomicFilterMode::BLL)
  {
    double a = std::pow(10.0, gainDb / 40.0);
    switch (mode)
    {
    case FBCytomicFilterMode::BLL: k = k / a; break;
    case FBCytomicFilterMode::LSH: g = g / std::sqrt(a); break;
    case FBCytomicFilterMode::HSH: g = g * std::sqrt(a); break;
    default: assert(false); break;
    }
  }

  _a1 = 1.0 / (1.0 + g * (g + k));
  _a2 = g * _a1;
  _a3 = g * _a2;

  _m0 = 0.0f;
  _m1 = 0.0f;
  _m2 = 0.0f;
  switch (mode)
  {
  case FBCytomicFilterMode::LPF: m2_ = 1.0; break;
  case FBCytomicFilterMode::BPF: m1_ = 1.0; break;
  case FBCytomicFilterMode::HPF: m0_ = 1.0; m1_ = -k; m2_ = -1.0; break;
  case FBCytomicFilterMode::BSF: m0_ = 1.0; m1_ = -k; break;
  case FBCytomicFilterMode::PEQ: m0_ = 1.0; m1_ = -k; m2_ = -2.0; break;
  case FBCytomicFilterMode::APF: m0_ = 1.0; m1_ = -2.0 * k; break;
  case FBCytomicFilterMode::BLL: m0_ = 1.0; m1_ = k * (a * a - 1.0); break;
  case FBCytomicFilterMode::LSH: m0_ = 1.0; m1_ = k * (a - 1.0); m2_ = a * a - 1.0; break;
  case FBCytomicFilterMode::HSH: m0_ = a * a; m1_ = k * (1.0 - a * a); m2_ = 1.0 - a * a; break;
  default: assert(false);  break;
  }
}