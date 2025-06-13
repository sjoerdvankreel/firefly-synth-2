#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

#include <firefly_base/base/topo/static/FBLog2Param.hpp>
#include <firefly_base/base/topo/static/FBBoolParam.hpp>
#include <firefly_base/base/topo/static/FBListParam.hpp>
#include <firefly_base/base/topo/static/FBBarsParam.hpp>
#include <firefly_base/base/topo/static/FBLinearParam.hpp>
#include <firefly_base/base/topo/static/FBIdentityParam.hpp>
#include <firefly_base/base/topo/static/FBDiscreteParam.hpp>
#include <firefly_base/base/topo/static/FBParamsDependencies.hpp>

#include <array>
#include <string>
#include <vector>
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
typedef std::function<std::string(int moduleSlot, int paramSlot)>
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

  FBParamNonRealTime const& NonRealTime() const;
  FBItemsParamNonRealTime const& ItemsNonRealTime() const;
  std::string GetDefaultText(int moduleSlot, int paramSlot) const;

  double DefaultNormalizedByText(int moduleSlot, int paramSlot) const;
  std::string NormalizedToTextWithUnit(bool io, double normalized) const;

  FBListParam& List() { FB_ASSERT(type == FBParamType::List); return list; }
  FBBarsParam& Bars() { FB_ASSERT(type == FBParamType::Bars); return bars; }
  FBLog2Param& Log2() { FB_ASSERT(type == FBParamType::Log2); return log2; }
  FBBoolParam& Boolean() { FB_ASSERT(type == FBParamType::Boolean); return boolean; }
  FBLinearParam& Linear() { FB_ASSERT(type == FBParamType::Linear); return linear; }
  FBDiscreteParam& Discrete() { FB_ASSERT(type == FBParamType::Discrete); return discrete; }
  FBIdentityParam& Identity() { FB_ASSERT(type == FBParamType::Identity); return identity; }

  FBListParam const& List() const { FB_ASSERT(type == FBParamType::List); return list; }
  FBBarsParam const& Bars() const { FB_ASSERT(type == FBParamType::Bars); return bars; }
  FBLog2Param const& Log2() const { FB_ASSERT(type == FBParamType::Log2); return log2; }
  FBBoolParam const& Boolean() const { FB_ASSERT(type == FBParamType::Boolean); return boolean; }
  FBLinearParam const& Linear() const { FB_ASSERT(type == FBParamType::Linear); return linear; }
  FBDiscreteParam const& Discrete() const { FB_ASSERT(type == FBParamType::Discrete); return discrete; }
  FBIdentityParam const& Identity() const { FB_ASSERT(type == FBParamType::Identity); return identity; }
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