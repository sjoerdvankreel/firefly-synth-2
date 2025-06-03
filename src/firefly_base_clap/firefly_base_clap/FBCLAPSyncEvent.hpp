#pragma once

inline int constexpr 
FBCLAPSyncEventReserve = 4096;

enum class FBCLAPSyncEventType
{
  BeginChange,
  EndChange,
  PerformEdit
};

struct FBCLAPSyncToMainEvent
{
  int paramIndex;
  float normalized;
};

struct FBCLAPSyncToAudioEvent
{
  int pad;
  int paramIndex;
  float normalized;
  FBCLAPSyncEventType type;
};

FBCLAPSyncToMainEvent
FBMakeSyncToMainEvent(int paramIndex, double normalized);
FBCLAPSyncToAudioEvent
FBMakeSyncToAudioEvent(FBCLAPSyncEventType type, int paramIndex, double normalized);
