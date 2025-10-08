#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBAccParamState.hpp>
#include <firefly_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <firefly_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <firefly_base/base/state/proc/FBVoiceBlockParamState.hpp>
#include <firefly_base/base/state/proc/FBGlobalBlockParamState.hpp>

// Sample accurate per voice also covers at-voice-start parameters.
enum class FBProcParamType
{
  VoiceAcc,
  GlobalAcc,
  VoiceBlock,
  GlobalBlock
};

inline bool 
FBProcParamTypeIsAcc(FBProcParamType type)
{
  return type == FBProcParamType::GlobalAcc || 
    type == FBProcParamType::VoiceAcc;
}

inline bool
FBProcParamTypeIsVoice(FBProcParamType type)
{
  return type == FBProcParamType::VoiceBlock || 
    type == FBProcParamType::VoiceAcc;
}

class FBProcParamState final
{
  FBProcParamType _type;
  union
  {
    FBVoiceAccParamState* _voiceAcc;
    FBGlobalAccParamState* _globalAcc;
    FBVoiceBlockParamState* _voiceBlock;
    FBGlobalBlockParamState* _globalBlock;
  };

  friend struct FBRuntimeTopo;
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBSmoothingProcessor;
  friend class FBProcStateContainer;

  FBVoiceBlockParamState& VoiceBlock();
  FBGlobalBlockParamState& GlobalBlock();

  void Value(float value);
  void InitProcessing(float value);
  void SetSmoothingCoeffs(int sampleCount);
  void InitProcessing(int voice, float value);

public:
  FB_COPY_MOVE_DEFCTOR(FBProcParamState);
  explicit FBProcParamState(FBVoiceAccParamState* voiceAcc);
  explicit FBProcParamState(FBGlobalAccParamState* globalAcc);
  explicit FBProcParamState(FBVoiceBlockParamState* voiceBlock);
  explicit FBProcParamState(FBGlobalBlockParamState* globalBlock);

  float Value() const;

  // public to allow modulation by the plug itself (aka modmatrix)
  FBVoiceAccParamState& VoiceAcc();
  FBGlobalAccParamState& GlobalAcc();

  FBVoiceAccParamState const& VoiceAcc() const;
  FBGlobalAccParamState const& GlobalAcc() const;
  FBVoiceBlockParamState const& VoiceBlock() const;
  FBGlobalBlockParamState const& GlobalBlock() const;

  FBProcParamType Type() const { return _type; }
  bool IsAcc() const { return FBProcParamTypeIsAcc(Type()); }
  bool IsVoice() const { return FBProcParamTypeIsVoice(Type()); }
};

inline FBProcParamState::
FBProcParamState(FBVoiceAccParamState* voiceAcc) :
_type(FBProcParamType::VoiceAcc),
_voiceAcc(voiceAcc) {}

inline FBProcParamState::
FBProcParamState(FBGlobalAccParamState* globalAcc) :
_type(FBProcParamType::GlobalAcc),
_globalAcc(globalAcc) {}

inline FBProcParamState::
FBProcParamState(FBVoiceBlockParamState* voiceBlock) :
_type(FBProcParamType::VoiceBlock),
_voiceBlock(voiceBlock) {}

inline FBProcParamState::
FBProcParamState(FBGlobalBlockParamState* globalBlock) :
_type(FBProcParamType::GlobalBlock), 
_globalBlock(globalBlock) {}

inline FBVoiceAccParamState&
FBProcParamState::VoiceAcc()
{
  FB_ASSERT(Type() == FBProcParamType::VoiceAcc);
  return *_voiceAcc;
}

inline FBVoiceAccParamState const&
FBProcParamState::VoiceAcc() const
{
  FB_ASSERT(Type() == FBProcParamType::VoiceAcc);
  return *_voiceAcc;
}

inline FBGlobalAccParamState&
FBProcParamState::GlobalAcc()
{
  FB_ASSERT(Type() == FBProcParamType::GlobalAcc);
  return *_globalAcc;
}

inline FBGlobalAccParamState const&
FBProcParamState::GlobalAcc() const
{
  FB_ASSERT(Type() == FBProcParamType::GlobalAcc);
  return *_globalAcc;
}

inline FBVoiceBlockParamState&
FBProcParamState::VoiceBlock()
{
  FB_ASSERT(Type() == FBProcParamType::VoiceBlock);
  return *_voiceBlock;
}

inline FBVoiceBlockParamState const&
FBProcParamState::VoiceBlock() const
{
  FB_ASSERT(Type() == FBProcParamType::VoiceBlock);
  return *_voiceBlock;
}

inline FBGlobalBlockParamState&
FBProcParamState::GlobalBlock()
{
  FB_ASSERT(Type() == FBProcParamType::GlobalBlock);
  return *_globalBlock;
}

inline FBGlobalBlockParamState const&
FBProcParamState::GlobalBlock() const
{
  FB_ASSERT(Type() == FBProcParamType::GlobalBlock);
  return *_globalBlock;
}

inline float
FBProcParamState::Value() const
{
  switch (Type())
  {
  case FBProcParamType::GlobalAcc: return GlobalAcc().Value();
  case FBProcParamType::GlobalBlock: return GlobalBlock().Value();
  case FBProcParamType::VoiceAcc: return VoiceAcc().GlobalValue();
  case FBProcParamType::VoiceBlock: return VoiceBlock().GlobalValue();
  default: FB_ASSERT(false); return 0.0f;
  }
}

inline void
FBProcParamState::Value(float value)
{
  // Actually normalized, but, just trying to find weird stuff.
  FB_ASSERT(-1.5f <= value && value <= 1.5f);
  FB_ASSERT(!std::isnan(value));
  switch (Type())
  {
  case FBProcParamType::GlobalAcc: GlobalAcc().Value(value); break;
  case FBProcParamType::GlobalBlock: GlobalBlock().Value(value); break;
  case FBProcParamType::VoiceAcc: VoiceAcc().GlobalValue(value); break;
  case FBProcParamType::VoiceBlock: VoiceBlock().GlobalValue(value); break;
  default: FB_ASSERT(false);
  }
}