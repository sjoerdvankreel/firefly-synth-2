#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>

#include <playground_base/base/topo/static/FBLog2Param.hpp>
#include <playground_base/base/topo/static/FBBoolParam.hpp>
#include <playground_base/base/topo/static/FBListParam.hpp>
#include <playground_base/base/topo/static/FBBarsParam.hpp>
#include <playground_base/base/topo/static/FBLinearParam.hpp>
#include <playground_base/base/topo/static/FBIdentityParam.hpp>
#include <playground_base/base/topo/static/FBDiscreteParam.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>

#include <array>
#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>
#include <functional>

class FBVoiceAccParamState;
class FBGlobalAccParamState;
class FBVoiceBlockParamState;
class FBGlobalBlockParamState;

enum class FBAutomationTiming { Never, PerSample, AtVoiceStart };

std::string
FBAutomationTimingToString(FBAutomationTiming timing);

typedef std::function<std::string(int slot)>
FBParamSlotFormatter;
typedef std::function<std::string(int slot)>
FBParamDefaultTextSelector;

typedef std::function<double* (
int moduleSlot, int paramSlot, void* state)>
FBScalarParamAddrSelector;

typedef std::function<float* (
int moduleSlot, int paramSlot, void* state)>
FBGlobalExchangeParamAddrSelector;
typedef std::function<std::array<float, FBMaxVoices>* (
int moduleSlot, int paramSlot, void* state)>
FBVoiceExchangeParamAddrSelector;

typedef std::function<FBVoiceAccParamState* (
int moduleSlot, int paramSlot, void* state)>
FBVoiceAccProcParamAddrSelector;
typedef std::function<FBGlobalAccParamState* (
int moduleSlot, int paramSlot, void* state)>
FBGlobalAccProcParamAddrSelector;
typedef std::function<FBVoiceBlockParamState* (
int moduleSlot, int paramSlot, void* state)>
FBVoiceBlockProcParamAddrSelector;
typedef std::function<FBGlobalBlockParamState* (
int moduleSlot, int paramSlot, void* state)>
FBGlobalBlockProcParamAddrSelector;

struct FBStaticParamBase
{
private:
  FBLog2ParamNonRealTime log2 = {};
  FBListParamNonRealTime list = {};
  FBBarsParamNonRealTime bars = {};
  FBBoolParamNonRealTime boolean = {};
  FBLinearParamNonRealTime linear = {};
  FBDiscreteParamNonRealTime discrete = {};
  FBIdentityParamNonRealTime identity = {};

public:
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string display = {};
  std::string defaultText = {};
  FBParamType type = (FBParamType)-1;
  FBParamsDependencies dependencies = {};
  FBParamSlotFormatter slotFormatter = {};
  FBParamDefaultTextSelector defaultTextSelector = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParamBase);

  std::string GetDefaultText(int slot) const;
  FBParamNonRealTime const& NonRealTime() const;
  FBItemsParamNonRealTime const& ItemsNonRealTime() const;

  double DefaultNormalizedByText(int slot) const;
  std::string NormalizedToTextWithUnit(bool io, double normalized) const;

  FBListParam& List() { assert(type == FBParamType::List); return list; }
  FBBarsParam& Bars() { assert(type == FBParamType::Bars); return bars; }
  FBLog2Param& Log2() { assert(type == FBParamType::Log2); return log2; }
  FBBoolParam& Boolean() { assert(type == FBParamType::Boolean); return boolean; }
  FBLinearParam& Linear() { assert(type == FBParamType::Linear); return linear; }
  FBDiscreteParam& Discrete() { assert(type == FBParamType::Discrete); return discrete; }
  FBIdentityParam& Identity() { assert(type == FBParamType::Identity); return identity; }

  FBListParam const& List() const { assert(type == FBParamType::List); return list; }
  FBBarsParam const& Bars() const { assert(type == FBParamType::Bars); return bars; }
  FBLog2Param const& Log2() const { assert(type == FBParamType::Log2); return log2; }
  FBBoolParam const& Boolean() const { assert(type == FBParamType::Boolean); return boolean; }
  FBLinearParam const& Linear() const { assert(type == FBParamType::Linear); return linear; }
  FBDiscreteParam const& Discrete() const { assert(type == FBParamType::Discrete); return discrete; }
  FBIdentityParam const& Identity() const { assert(type == FBParamType::Identity); return identity; }
};

struct FBStaticGUIParam final:
public FBStaticParamBase
{
public:
  FBScalarParamAddrSelector scalarAddr = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticGUIParam);
};

struct FBStaticParam final:
public FBStaticParamBase
{
public:
  bool acc = false;
  bool output = false;

  FBAutomationTiming AutomationTiming() const;
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);

  FBScalarParamAddrSelector scalarAddr = {};
  FBVoiceExchangeParamAddrSelector voiceExchangeAddr = {};
  FBGlobalExchangeParamAddrSelector globalExchangeAddr = {};
  FBVoiceAccProcParamAddrSelector voiceAccProcAddr = {};
  FBGlobalAccProcParamAddrSelector globalAccProcAddr = {};
  FBVoiceBlockProcParamAddrSelector voiceBlockProcAddr = {};
  FBGlobalBlockProcParamAddrSelector globalBlockProcAddr = {};

  bool IsAcc() const { return IsVoiceAcc() || IsGlobalAcc(); }
  bool IsVoice() const { return IsVoiceAcc() || IsVoiceBlock(); }
  bool IsVoiceAcc() const { return voiceAccProcAddr != nullptr; }
  bool IsGlobalAcc() const { return globalAccProcAddr != nullptr; }
  bool IsVoiceBlock() const { return voiceBlockProcAddr != nullptr; }
  bool IsGlobalBlock() const { return globalBlockProcAddr != nullptr; }
};