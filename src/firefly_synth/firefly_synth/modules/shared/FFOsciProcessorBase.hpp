#pragma once

#include <firefly_synth/modules/shared/FFOsciTopoBase.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>

class FBAccParamState;
struct FBModuleProcState;

// todo drop
class FFOsciProcessorBase
{
protected:
  float _key = {};
  int _uniCount = {};
  FBSArray<float, FFOsciBaseUniMaxCount> _uniPosMHalfToHalf = {};
  FBSArray<float, FFOsciBaseUniMaxCount> _uniPosAbsHalfToHalf = {};

  FBSArray<float, FBFixedBlockSamples> _gainPlain = {};
  FBSArray<float, FBFixedBlockSamples> _uniBlendPlain = {};
  FBSArray<float, FBFixedBlockSamples> _uniSpreadPlain = {};
  FBSArray2<float, FBFixedBlockSamples, FFOsciBaseUniMaxCount> _uniOutput = {};

  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessorBase);

  void BeginVoice(FBModuleProcState& state, int uniCount);
  void ProcessGainSpreadBlend(FBSArray2<float, FBFixedBlockSamples, 2>& output);
};