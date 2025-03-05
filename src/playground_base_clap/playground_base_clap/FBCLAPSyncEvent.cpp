#include <playground_base_clap/FBCLAPSyncEvent.hpp>

FBCLAPSyncToMainEvent
FBMakeSyncToMainEvent(
  int paramIndex, double normalized)
{
  FBCLAPSyncToMainEvent result;
  result.paramIndex = paramIndex;
  result.normalized = static_cast<float>(normalized);
  return result;
}

FBCLAPSyncToAudioEvent
FBMakeSyncToAudioEvent(
  FBCLAPSyncEventType type, int paramIndex, double normalized)
{
  FBCLAPSyncToAudioEvent result;
  result.type = type;
  result.paramIndex = paramIndex;
  result.normalized = static_cast<float>(normalized);
  return result;
}