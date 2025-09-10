#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

#include <firefly_base/base/topo/static/FBLog2Param.hpp>
#include <firefly_base/base/topo/static/FBBoolParam.hpp>
#include <firefly_base/base/topo/static/FBListParam.hpp>
#include <firefly_base/base/topo/static/FBBarsParam.hpp>
#include <firefly_base/base/topo/static/FBLinearParam.hpp>
#include <firefly_base/base/topo/static/FBSlotFormatter.hpp>
#include <firefly_base/base/topo/static/FBIdentityParam.hpp>
#include <firefly_base/base/topo/static/FBDiscreteParam.hpp>
#include <firefly_base/base/topo/static/FBParamsDependencies.hpp>

#include <array>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <functional>

struct FBStaticTopo;

class FBVoiceAccParamState;
class FBGlobalAccParamState;
class FBVoiceBlockParamState;
class FBGlobalBlockParamState;

// VoiceStart: sample-accurate from the host side, snapshot at voice start on the plug side.
// Bit wasteful, but allows to reuse existing machinery for modulation. And, there's not many of those params.
// Fake: i use it to provide a toggle which we check on realtime if changed, to flush delay lines.
// Accurate: sample-accurate, per-voice if the module is per-voice.
// Block: update per block for global, snap to global value at voice start for voice (so more efficient than VoiceStart, but not modulatable).
enum class FBParamMode { Block, Accurate, VoiceStart, Output, Fake };

typedef std::function<std::string(int moduleIndex, int moduleSlot, int paramSlot)>
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
  std::string matrixName = {};
  std::string defaultText = {};
  bool slotFormatDisplay = {};
  bool slotFormatterOverrides = {};
  FBParamType type = (FBParamType)-1;
  FBParamsDependencies dependencies = {};
  FBModuleItemSlotFormatter slotFormatter = {};
  FBParamDefaultTextSelector defaultTextSelector = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParamBase);

  std::string DebugNameAndId() const;
  FBParamNonRealTime const& NonRealTime() const;
  FBItemsParamNonRealTime const& ItemsNonRealTime() const;

  std::string GetDefaultText(int moduleIndex, int moduleSlot, int paramSlot) const;
  double DefaultNormalizedByText(int moduleIndex, int moduleSlot, int paramSlot) const;
  std::string NormalizedToTextWithUnit(bool io, int moduleIndex, double normalized) const;
  std::optional<double> TextToPlain(bool io, int moduleIndex, std::string const& text) const;
  std::optional<double> TextToNormalized(bool io, int moduleIndex, std::string const& text) const;

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

struct FBStaticGUIParam final :
  public FBStaticParamBase
{
public:
  bool IsOutput() const { return false; }
  FBScalarParamAddrSelector scalarAddr = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticGUIParam);
};

struct FBStaticParam final :
  public FBStaticParamBase
{
public:
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);

  FBParamMode mode = {};
  FBScalarParamAddrSelector scalarAddr = {};
  FBVoiceExchangeParamAddrSelector voiceExchangeAddr = {};
  FBGlobalExchangeParamAddrSelector globalExchangeAddr = {};
  FBVoiceAccProcParamAddrSelector voiceAccProcAddr = {};
  FBGlobalAccProcParamAddrSelector globalAccProcAddr = {};
  FBVoiceBlockProcParamAddrSelector voiceBlockProcAddr = {};
  FBGlobalBlockProcParamAddrSelector globalBlockProcAddr = {};

  bool IsOutput() const { return mode == FBParamMode::Output; }
  bool IsAcc() const { return IsVoiceAcc() || IsGlobalAcc(); }
  bool IsVoice() const { return IsVoiceAcc() || IsVoiceBlock(); }
  bool IsVoiceAcc() const { return voiceAccProcAddr != nullptr; }
  bool IsGlobalAcc() const { return globalAccProcAddr != nullptr; }
  bool IsVoiceBlock() const { return voiceBlockProcAddr != nullptr; }
  bool IsGlobalBlock() const { return globalBlockProcAddr != nullptr; }
};