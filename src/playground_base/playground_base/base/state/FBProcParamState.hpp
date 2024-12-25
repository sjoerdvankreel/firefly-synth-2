#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBVoiceBlockParamState.hpp>
#include <playground_base/base/state/FBGlobalBlockParamState.hpp>

#include <cassert>

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
  friend class FBSmoothProcessor;
  friend class FBProcStateContainer;

  FBVoiceAccParamState& VoiceAcc();
  FBGlobalAccParamState& GlobalAcc();
  FBVoiceBlockParamState& VoiceBlock();
  FBGlobalBlockParamState& GlobalBlock();

  void Value(float value);
  void SetSmoothingCoeffs(float sampleRate, float durationSecs);

public:
  FB_COPY_MOVE_DEFCTOR(FBProcParamState);
  explicit FBProcParamState(FBVoiceAccParamState* voiceAcc);
  explicit FBProcParamState(FBGlobalAccParamState* globalAcc);
  explicit FBProcParamState(FBVoiceBlockParamState* voiceBlock);
  explicit FBProcParamState(FBGlobalBlockParamState* globalBlock);

  float Value() const;
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
  assert(Type() == FBProcParamType::VoiceAcc);
  return *_voiceAcc;
}

inline FBVoiceAccParamState const&
FBProcParamState::VoiceAcc() const
{
  assert(Type() == FBProcParamType::VoiceAcc);
  return *_voiceAcc;
}

inline FBGlobalAccParamState&
FBProcParamState::GlobalAcc()
{
  assert(Type() == FBProcParamType::GlobalAcc);
  return *_globalAcc;
}

inline FBGlobalAccParamState const&
FBProcParamState::GlobalAcc() const
{
  assert(Type() == FBProcParamType::GlobalAcc);
  return *_globalAcc;
}

inline FBVoiceBlockParamState&
FBProcParamState::VoiceBlock()
{
  assert(Type() == FBProcParamType::VoiceBlock);
  return *_voiceBlock;
}

inline FBVoiceBlockParamState const&
FBProcParamState::VoiceBlock() const
{
  assert(Type() == FBProcParamType::VoiceBlock);
  return *_voiceBlock;
}

inline FBGlobalBlockParamState&
FBProcParamState::GlobalBlock()
{
  assert(Type() == FBProcParamType::GlobalBlock);
  return *_globalBlock;
}

inline FBGlobalBlockParamState const&
FBProcParamState::GlobalBlock() const
{
  assert(Type() == FBProcParamType::GlobalBlock);
  return *_globalBlock;
}

inline float
FBProcParamState::Value() const
{
  switch (Type())
  {
  case FBProcParamType::VoiceAcc: return VoiceAcc().Value();
  case FBProcParamType::GlobalAcc: return GlobalAcc().Value();
  case FBProcParamType::VoiceBlock: return VoiceBlock().Value();
  case FBProcParamType::GlobalBlock: return GlobalBlock().Value();
  default: assert(false); return 0.0f;
  }
}

inline void
FBProcParamState::Value(float value)
{
  switch (Type())
  {
  case FBProcParamType::VoiceAcc: VoiceAcc().Value(value); break;
  case FBProcParamType::GlobalAcc: GlobalAcc().Value(value); break;
  case FBProcParamType::VoiceBlock: VoiceBlock().Value(value); break;
  case FBProcParamType::GlobalBlock: GlobalBlock().Value(value); break;
  default: assert(false);
  }
}