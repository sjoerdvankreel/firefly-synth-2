#pragma once

#include <memory>

struct FBStaticModule;

enum class FFSettingsSoftClipType { Off, Db0, Db6, Db12 };

enum class FFSettingsParam { 
  HostSmoothTime, ReceiveNotes, AutoSoftClip,
  Tuning, TuneOnNote, 
  TuneMasterPB, TuneMasterMatrix,
  TuneVoiceCoarse, TuneVoiceMatrix, Count };

std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool isFx);