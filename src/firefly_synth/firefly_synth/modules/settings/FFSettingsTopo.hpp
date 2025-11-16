#pragma once

#include <memory>

struct FBStaticModule;
enum class FFSettingsParam { 
  HostSmoothTime, ReceiveNotes,
  Tuning, TuneOnNote, 
  TuneMasterPB, TuneMasterMatrix,
  TuneVoiceCoarse, TuneVoiceMatrix, Count };
std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool isFx);