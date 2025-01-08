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
  int pad;
  int paramIndex;
  double normalized;
};

struct FBCLAPSyncToAudioEvent
{
  double normalized;
  int paramIndex;
  FBCLAPSyncEventType type;
};
