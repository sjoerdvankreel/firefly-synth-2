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
  double value;
};

struct FBCLAPSyncToAudioEvent
{
  double value;
  int paramIndex;
  FBCLAPSyncEventType type;
};
