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
  FBArray<float, FFOsciBaseUniMaxCount> _uniPosMHalfToHalf = {};
  FBArray<float, FFOsciBaseUniMaxCount> _uniPosAbsHalfToHalf = {};

  FBArray<float, FBFixedBlockSamples> _gainPlain = {};
  FBArray<float, FBFixedBlockSamples> _uniBlendPlain = {};
  FBArray<float, FBFixedBlockSamples> _uniSpreadPlain = {};
  FBArray2<float, FBFixedBlockSamples, FFOsciBaseUniMaxCount> _uniOutput = {};

  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessorBase);

  void BeginVoice(FBModuleProcState& state, int uniCount);
  void ProcessGainSpreadBlend(FBArray2<float, FBFixedBlockSamples, 2>& output);
};