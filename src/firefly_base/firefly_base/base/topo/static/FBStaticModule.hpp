#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBStaticParam.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <functional>

class FBPlugGUI;
struct FBModuleGraphComponentData;
struct FBModuleProcExchangeStateBase;

typedef std::function<FBModuleProcExchangeStateBase* (
  int moduleSlot, void* state)>
FBGlobalModuleExchangeAddrSelector;
typedef std::function<FBModuleProcExchangeStateBase* (
  int voice, int moduleSlot, void* state)>
FBVoiceModuleExchangeAddrSelector;
typedef std::function<void(FBModuleGraphComponentData* graphData)>
FBModuleGraphRenderer;

struct FBStaticModule final
{
  bool voice = false;
  int graphCount = 0;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string tabName = {};
  std::string graphName = {};
  std::vector<FBStaticParam> params = {};
  std::vector<FBStaticGUIParam> guiParams = {};
  FBModuleGraphRenderer graphRenderer = {};
  FBVoiceModuleExchangeAddrSelector voiceModuleExchangeAddr = {};
  FBGlobalModuleExchangeAddrSelector globalModuleExchangeAddr = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);

  template <class T, class ParamIndex>
  T NormalizedToListFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  bool NormalizedToBoolFast(ParamIndex index, float normalized) const;

  template <class ParamIndex>
  float DiscreteToNormalizedFast(ParamIndex index, int plain) const;
  template <class ParamIndex>
  int NormalizedToDiscreteFast(ParamIndex index, float normalized) const;

  template <class ParamIndex>
  int NormalizedToBarsFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  float NormalizedToBarsFreqFast(ParamIndex index, float normalized, float bpm) const;
  template <class ParamIndex>
  int NormalizedToBarsSamplesFast(ParamIndex index, float normalized, float sampleRate, float bpm) const;

  template <class ParamIndex>
  float NormalizedToLinearFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  FBBatch<float> NormalizedToLinearFast(ParamIndex index, FBBatch<float> normalized) const;
  template <class ParamIndex>
  FBBatch<double> NormalizedToLinearFast(ParamIndex index, FBBatch<double> normalized) const;
  template <class ParamIndex>
  FBBatch<float> NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, int pos) const;

  template <class ParamIndex>
  int NormalizedToLinearTimeSamplesFast(ParamIndex index, float normalized, float sampleRate) const;
  template <class ParamIndex>
  int NormalizedToLinearFreqSamplesFast(ParamIndex index, float normalized, float sampleRate) const;

  template <class ParamIndex>
  float NormalizedToLog2Fast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  FBBatch<float> NormalizedToLog2Fast(ParamIndex index, FBBatch<float> normalized) const;
  template <class ParamIndex>
  FBBatch<double> NormalizedToLog2Fast(ParamIndex index, FBBatch<double> normalized) const;
  template <class ParamIndex>
  FBBatch<float> NormalizedToLog2Fast(ParamIndex index, FBAccParamState const& normalized, int pos) const;

  template <class ParamIndex>
  int NormalizedToLog2TimeSamplesFast(ParamIndex index, float normalized, float sampleRate) const;
  template <class ParamIndex>
  int NormalizedToLog2FreqSamplesFast(ParamIndex index, float normalized, float sampleRate) const;

  template <class ParamIndex>
  float NormalizedToIdentityFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  FBBatch<float> NormalizedToIdentityFast(ParamIndex index, FBBatch<float> normalized) const;
  template <class ParamIndex>
  FBBatch<double> NormalizedToIdentityFast(ParamIndex index, FBBatch<double> normalized) const;
  template <class ParamIndex>
  FBBatch<float> NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, int pos) const;
};

template <class ParamIndex> 
inline bool
FBStaticModule::NormalizedToBoolFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Boolean().NormalizedToPlainFast(normalized);
}

template <class T, class ParamIndex>
inline T
FBStaticModule::NormalizedToListFast(ParamIndex index, float normalized) const
{
  return static_cast<T>(params[static_cast<int>(index)].List().NormalizedToPlainFast(normalized));
}

template <class ParamIndex>
inline float 
FBStaticModule::DiscreteToNormalizedFast(ParamIndex index, int plain) const
{
  return params[static_cast<int>(index)].Discrete().PlainToNormalizedFast(plain);
}

template <class ParamIndex>
inline int 
FBStaticModule::NormalizedToDiscreteFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Discrete().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline int 
FBStaticModule::NormalizedToBarsFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Bars().NormalizedToPlainFast(normalized);
}

template <class ParamIndex> 
inline float 
FBStaticModule::NormalizedToBarsFreqFast(ParamIndex index, float normalized, float bpm) const
{
  return params[static_cast<int>(index)].Bars().NormalizedToFreqFast(normalized, bpm);
}

template <class ParamIndex>
inline int
FBStaticModule::NormalizedToBarsSamplesFast(ParamIndex index, float normalized, float sampleRate, float bpm) const
{
  return params[static_cast<int>(index)].Bars().NormalizedToSamplesFast(normalized, sampleRate, bpm);
}

template <class ParamIndex>
inline float 
FBStaticModule::NormalizedToLinearFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Linear().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<float>
FBStaticModule::NormalizedToLinearFast(ParamIndex index, FBBatch<float> normalized) const
{
  return params[static_cast<int>(index)].Linear().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<double>
FBStaticModule::NormalizedToLinearFast(ParamIndex index, FBBatch<double> normalized) const
{
  return params[static_cast<int>(index)].Linear().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<float>
FBStaticModule::NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, int pos) const
{
  return params[static_cast<int>(index)].Linear().NormalizedToPlainFast(normalized, pos);
}

template <class ParamIndex>
inline int 
FBStaticModule::NormalizedToLinearTimeSamplesFast(ParamIndex index, float normalized, float sampleRate) const
{
  return params[static_cast<int>(index)].Linear().NormalizedTimeToSamplesFast(normalized, sampleRate);
}

template <class ParamIndex>
inline int
FBStaticModule::NormalizedToLinearFreqSamplesFast(ParamIndex index, float normalized, float sampleRate) const
{
  return params[static_cast<int>(index)].Linear().NormalizedFreqToSamplesFast(normalized, sampleRate);
}

template <class ParamIndex>
inline float
FBStaticModule::NormalizedToLog2Fast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Log2().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<float>
FBStaticModule::NormalizedToLog2Fast(ParamIndex index, FBBatch<float> normalized) const
{
  return params[static_cast<int>(index)].Log2().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<double>
FBStaticModule::NormalizedToLog2Fast(ParamIndex index, FBBatch<double> normalized) const
{
  return params[static_cast<int>(index)].Log2().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<float>
FBStaticModule::NormalizedToLog2Fast(ParamIndex index, FBAccParamState const& normalized, int pos) const
{
  return params[static_cast<int>(index)].Log2().NormalizedToPlainFast(normalized, pos);
}

template <class ParamIndex>
inline int
FBStaticModule::NormalizedToLog2TimeSamplesFast(ParamIndex index, float normalized, float sampleRate) const
{
  return params[static_cast<int>(index)].Log2().NormalizedTimeToSamplesFast(normalized, sampleRate);
}

template <class ParamIndex>
inline int
FBStaticModule::NormalizedToLog2FreqSamplesFast(ParamIndex index, float normalized, float sampleRate) const
{
  return params[static_cast<int>(index)].Log2().NormalizedFreqToSamplesFast(normalized, sampleRate);
}

template <class ParamIndex>
inline float FBStaticModule::NormalizedToIdentityFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Identity().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<float>
FBStaticModule::NormalizedToIdentityFast(ParamIndex index, FBBatch<float> normalized) const
{
  return params[static_cast<int>(index)].Identity().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<double>
FBStaticModule::NormalizedToIdentityFast(ParamIndex index, FBBatch<double> normalized) const
{
  return params[static_cast<int>(index)].Identity().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline FBBatch<float>
FBStaticModule::NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, int pos) const
{
  return params[static_cast<int>(index)].Identity().NormalizedToPlainFast(normalized, pos);
}
