#pragma once

#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <cmath>
#include <numbers>
#include <cassert>

enum class FBCytomicFilterMode
{ LPF, BPF, HPF, BSF, APF, PEQ, BLL, LSH, HSH };

template <int Channels>
class FBCytomicFilter final
{
  double _m0 = {};
  double _m1 = {};
  double _m2 = {};
  double _a1 = {};
  double _a2 = {};
  double _a3 = {};
  std::array<double, Channels> _ic1eq = {};
  std::array<double, Channels> _ic2eq = {};

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
  double v3 = v0 - this->_ic2eq[channel];
  double v1 = this->_a1 * this->_ic1eq[channel] + this->_a2 * v3;
  double v2 = this->_ic2eq[channel] + this->_a2 * this->_ic1eq[channel] + this->_a3 * v3;
  this->_ic1eq[channel] = 2 * v1 - this->_ic1eq[channel];
  this->_ic2eq[channel] = 2 * v2 - this->_ic2eq[channel];
  return this->_m0 * v0 + this->_m1 * v1 + this->_m2 * v2;
}

template <int Channels>
inline void 
FBCytomicFilter<Channels>::Set(
  FBCytomicFilterMode mode, double sampleRate, 
  double freqHz, double resNorm, double gainDb)
{
  double a;
  double k = 2.0 - 2.0 * resNorm;
  double g = std::tan(std::numbers::pi * freqHz / sampleRate);

  if (mode >= FBCytomicFilterMode::BLL)
  {
    a = std::pow(10.0, gainDb / 40.0);
    switch (mode)
    {
    case FBCytomicFilterMode::BLL: k = k / a; break;
    case FBCytomicFilterMode::LSH: g = g / std::sqrt(a); break;
    case FBCytomicFilterMode::HSH: g = g * std::sqrt(a); break;
    default: assert(false); break;
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
  case FBCytomicFilterMode::LPF: this->_m2 = 1.0; break;
  case FBCytomicFilterMode::BPF: this->_m1 = 1.0; break;
  case FBCytomicFilterMode::HPF: this->_m0 = 1.0; this->_m1 = -k; this->_m2 = -1.0; break;
  case FBCytomicFilterMode::BSF: this->_m0 = 1.0; this->_m1 = -k; break;
  case FBCytomicFilterMode::PEQ: this->_m0 = 1.0; this->_m1 = -k; this->_m2 = -2.0; break;
  case FBCytomicFilterMode::APF: this->_m0 = 1.0; this->_m1 = -2.0 * k; break;
  case FBCytomicFilterMode::BLL: this->_m0 = 1.0; this->_m1 = k * (a * a - 1.0); break;
  case FBCytomicFilterMode::LSH: this->_m0 = 1.0; this->_m1 = k * (a - 1.0); this->_m2 = a * a - 1.0; break;
  case FBCytomicFilterMode::HSH: this->_m0 = a * a; this->_m1 = k * (1.0 - a * a); this->_m2 = 1.0 - a * a; break;
  default: assert(false);  break;
  }
}