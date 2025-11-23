#pragma once

#include <vector>
#include <string>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

int FFEnvIndexOfMSEGSnapXCount(int count); 
int FFEnvIndexOfMSEGSnapYCount(int count);
std::vector<int> FFEnvMakeMSEGSnapXCounts();
std::vector<int> FFEnvMakeMSEGSnapYCounts();

inline int constexpr FFEnvStageCount = 15;

// Per-stage.
inline int constexpr FFEnvMinBarsNum = 1;
inline int constexpr FFEnvMinBarsDen = 128;
inline int constexpr FFEnvMaxBarsNum = 4;
inline int constexpr FFEnvMaxBarsDen = 1;
inline float constexpr FFEnvMaxTime = 10.0f;

enum class FFEnvCVOutput { Output, Count };
enum class FFEnvType { Off, Linear, Exp };
std::string FFEnvTypeToString(FFEnvType type);

enum class FFEnvGUIParam { 
  MSEGXEditMode, MSEGSnapXCount, 
  MSEGYEditMode, MSEGSnapYCount,
  Count };

enum class FFEnvParam {
  Type, Sync, Release, SmoothTime, SmoothBars, 
  LoopStart, LoopLength, StartLevel,
  StageLevel, StageSlope, StageTime, StageBars, Count };