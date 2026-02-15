#pragma once

#include <firefly_base/base/topo/static/FBBarsItem.hpp>

#include <vector>
#include <string>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

int FFEnvIndexOfMSEGSnapXCount(int count); 
int FFEnvIndexOfMSEGSnapYCount(int count);
std::vector<int> FFEnvMakeMSEGSnapXCounts();
std::vector<int> FFEnvMakeMSEGSnapYCounts();
std::vector<FBBarsItem> FFEnvMakeBarsItems();

inline int constexpr FFEnvStageCount = 16;

// Per-stage.
inline int constexpr FFEnvMinBarsNum = 1;
inline int constexpr FFEnvMinBarsDen = 128;
inline int constexpr FFEnvMaxBarsNum = 4;
inline int constexpr FFEnvMaxBarsDen = 1;
inline float constexpr FFEnvMaxTime = 10.0f;

enum class FFEnvCVOutput { Output, Count };
enum class FFEnvType { Off, Linear, Exp };
enum class EnvSection { AttackDecay, Loop, Release, All };
std::string FFEnvTypeToString(FFEnvType type);

enum class FFEnvGUIParam { 
  MSEGXEditMode, MSEGSnapXCount, 
  MSEGYEditMode, MSEGSnapYCount,
  Count };

enum class FFEnvParam {
  Type, Sync, Release, SmoothTime, SmoothBars, 
  LoopStart, LoopLength, StartLevel,
  StageLevel, StageSlope, StageTime, StageBars, Count };

struct FFEnvSectionDetails
{
  int stageEnd = {};
  int stageStart = {};
  bool haveSection = {};
  int sectionStartSamples = {};
  int sectionLengthSamples = {};
  float sectionLengthSeconds = {};
  std::vector<int> stageLengths = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FFEnvSectionDetails);
};

struct FFEnvDetails
{
  int smoothLengthSamples = {};
  float smoothLengthSeconds = {};
  FFEnvSectionDetails all = {};
  FFEnvSectionDetails loop = {};
  FFEnvSectionDetails release = {};
  FFEnvSectionDetails attackDecay = {};
  
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FFEnvDetails);
  FFEnvSectionDetails const& GetSectionDetails(EnvSection section);
};

inline FFEnvSectionDetails const&
FFEnvDetails::GetSectionDetails(EnvSection section)
{
  switch (section)
  {
  case EnvSection::All: return all;
  case EnvSection::Loop: return loop;
  case EnvSection::Release: return release;
  case EnvSection::AttackDecay: return attackDecay;
  default: FB_ASSERT(false); return *((FFEnvSectionDetails*)(nullptr));
  }
}