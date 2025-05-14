#pragma once

#include <playground_plug/modules/osci_base/FFOsciTopoBase.hpp>
#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFOsciProcessorBase
{
protected:
  float _key = {};
  int _uniCount = {};
  FBSIMDArray<float, FFOsciBaseUniMaxCount> _uniPosMHalfToHalf = {};
  FBSIMDArray<float, FFOsciBaseUniMaxCount> _uniPosAbsHalfToHalf = {};

  FBSIMDArray<float, FBFixedBlockSamples> _gainPlain = {};
  FBSIMDArray<float, FBFixedBlockSamples> _uniBlendPlain = {};
  FBSIMDArray<float, FBFixedBlockSamples> _uniSpreadPlain = {};
  FBSIMDArray2<float, FBFixedBlockSamples, FFOsciBaseUniMaxCount> _uniOutput = {};

  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessorBase);

  void BeginVoice(FBModuleProcState& state, int uniCount);
  void ProcessGainSpreadBlend(FBSIMDArray2<float, FBFixedBlockSamples, 2>& output);
};