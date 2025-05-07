#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/static/FBStaticGUIParam.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <functional>

class FBPlugGUI;
struct FBModuleGraphComponentData;
struct FBModuleProcExchangeState;

typedef std::function<FBModuleProcExchangeState* (
  int moduleSlot, void* state)>
FBGlobalModuleExchangeAddrSelector;
typedef std::function<FBModuleProcExchangeState* (
  int voice, int moduleSlot, void* state)>
FBVoiceModuleExchangeAddrSelector;
typedef std::function<void(FBModuleGraphComponentData* graphData)>
FBModuleGraphRenderer;

struct FBModuleAddrSelectors final
{
  FB_COPY_MOVE_DEFCTOR(FBModuleAddrSelectors);
  FBVoiceModuleExchangeAddrSelector voiceModuleExchange = {};
  FBGlobalModuleExchangeAddrSelector globalModuleExchange = {};
};

struct FBStaticModule final
{
  bool voice = false;
  int graphCount = 1;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  FBModuleAddrSelectors addrSelectors = {};
  FBModuleGraphRenderer graphRenderer = {};
  std::vector<FBStaticParam> params = {};
  std::vector<FBStaticGUIParam> guiParams = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);

  template <class T, class ParamIndex>
  T NormalizedToListFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  int NormalizedToNoteFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  bool NormalizedToBoolFast(ParamIndex index, float normalized) const;

  template <class ParamIndex>
  float DiscreteToNormalizedFast(ParamIndex index, int plain) const;
  template <class ParamIndex>
  int NormalizedToDiscreteFast(ParamIndex index, float normalized) const;

  template <class ParamIndex>
  int NormalizedToBarsFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  int NormalizedToBarsSamplesFast(ParamIndex index, float normalized, float sampleRate, float bpm) const;

  template <class ParamIndex>
  float NormalizedToLinearFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  FBSIMDVector<float> NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, int pos) const;

  // todo drop ?
  template <class ParamIndex>
  void NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, FBFixedFloatArray& plain) const;
  template <class ParamIndex>
  void NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, FBFixedDoubleArray& plain) const;

  template <class ParamIndex>
  int NormalizedToLinearTimeSamplesFast(ParamIndex index, float normalized, float sampleRate) const;
  template <class ParamIndex>
  int NormalizedToLinearFreqSamplesFast(ParamIndex index, float normalized, float sampleRate) const;

  template <class ParamIndex>
  float NormalizedToLog2Fast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  FBSIMDVector<float> NormalizedToLog2Fast(ParamIndex index, FBAccParamState const& normalized, int pos) const;
  template <class ParamIndex>
  void NormalizedToLog2Fast(ParamIndex index, FBAccParamState const& normalized, FBFixedFloatArray& plain) const;
  template <class ParamIndex>
  void NormalizedToLog2Fast(ParamIndex index, FBAccParamState const& normalized, FBFixedDoubleArray& plain) const;

  template <class ParamIndex>
  int NormalizedToLog2TimeSamplesFast(ParamIndex index, float normalized, float sampleRate) const;
  template <class ParamIndex>
  int NormalizedToLog2FreqSamplesFast(ParamIndex index, float normalized, float sampleRate) const;

  template <class ParamIndex>
  float NormalizedToIdentityFast(ParamIndex index, float normalized) const;
  template <class ParamIndex>
  FBSIMDVector<float> NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, int pos) const;
  template <class ParamIndex>
  void NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, FBFixedFloatArray& plain) const;
  template <class ParamIndex>
  void NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, FBFixedDoubleArray& plain) const;
};

template <class ParamIndex> 
inline bool
FBStaticModule::NormalizedToBoolFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Boolean().NormalizedToPlainFast(normalized);
}

template <class ParamIndex>
inline int
FBStaticModule::NormalizedToNoteFast(ParamIndex index, float normalized) const
{
  return params[static_cast<int>(index)].Note().NormalizedToPlainFast(normalized);
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
inline FBSIMDVector<float> 
FBStaticModule::NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, int pos) const
{
  return params[static_cast<int>(index)].Linear().NormalizedToPlainFast(normalized, pos);
}

template <class ParamIndex>
inline void 
FBStaticModule::NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, FBFixedFloatArray& plain) const
{
  params[static_cast<int>(index)].Linear().NormalizedToPlainFast(normalized, plain);
}

template <class ParamIndex>
inline void
FBStaticModule::NormalizedToLinearFast(ParamIndex index, FBAccParamState const& normalized, FBFixedDoubleArray& plain) const
{
  params[static_cast<int>(index)].Linear().NormalizedToPlainFast(normalized, plain);
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
inline void
FBStaticModule::NormalizedToLog2Fast(ParamIndex index, FBAccParamState const& normalized, FBFixedFloatArray& plain) const
{
  params[static_cast<int>(index)].Log2().NormalizedToPlainFast(normalized, plain);
}

template <class ParamIndex>
inline void
FBStaticModule::NormalizedToLog2Fast(ParamIndex index, FBAccParamState const& normalized, FBFixedDoubleArray& plain) const
{
  params[static_cast<int>(index)].Log2().NormalizedToPlainFast(normalized, plain);
}

template <class ParamIndex>
inline FBSIMDVector<float>
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
inline FBSIMDVector<float> 
FBStaticModule::NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, int pos) const
{
  return params[static_cast<int>(index)].Identity().NormalizedToPlainFast(normalized, pos);
}

template <class ParamIndex>
void FBStaticModule::NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, FBFixedFloatArray& plain) const
{
  params[static_cast<int>(index)].Identity().NormalizedToPlainFast(normalized, plain);
}

template <class ParamIndex>
void FBStaticModule::NormalizedToIdentityFast(ParamIndex index, FBAccParamState const& normalized, FBFixedDoubleArray& plain) const
{
  params[static_cast<int>(index)].Identity().NormalizedToPlainFast(normalized, plain);
}
