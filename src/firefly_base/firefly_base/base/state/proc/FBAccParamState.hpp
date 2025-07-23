#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>

#include <array>
#include <algorithm>

class alignas(FBSIMDAlign) FBAccParamState final
{
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBSmoothingProcessor;
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  float _hostModulation = {};
  FBBasicLPFilter _hostSmoother = {};
  FBSArray<float, FBFixedBlockSamples> _hostCV = {};
  FBSArray<float, FBFixedBlockSamples> const* _modulatedByPlugCV;

  void InitProcessing(float value) { _hostCV.Fill(value); }
  void ClearPlugModulation() { _modulatedByPlugCV = &_hostCV; }
  void ModulateByHost(float offset) { _hostModulation = offset; }
  void SetSmoothingCoeffs(int sampleCount) { _hostSmoother.SetCoeffs(sampleCount); }
  
  void SmoothNextHostValue(int sample, float automation) 
  { _hostCV.Set(sample, _hostSmoother.Next(std::clamp(automation + _hostModulation, 0.0f, 1.0f))); }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAccParamState);
  FBAccParamState() : _modulatedByPlugCV(&_hostCV) {}

  float Last() const { return CV().Get(FBFixedBlockSamples - 1); }
  FBSArray<float, FBFixedBlockSamples> const& CV() const { return *_modulatedByPlugCV; }
  void ApplyPlugModulation(FBSArray<float, FBFixedBlockSamples> const* modulatedByPlugCV) { _modulatedByPlugCV = modulatedByPlugCV; }
};