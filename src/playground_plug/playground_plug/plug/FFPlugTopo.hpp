#pragma once

#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

inline int constexpr FFGLFOCount = 1;
inline int constexpr FFOsciCount = 2; // todo oscis - it must be a single module
inline int constexpr FFOsciGainCount = 2;

enum { FFModuleGLFO, FFModuleOsci, FFModuleMaster, FFModuleCount };
enum { FFGLFOBlockOn, FFGLFOAccRate, FFGLFOParamCount };
enum { FFMasterBlockSmooth, FFMasterAccGain, FFMasterParamCount };
enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypePulse, FFOsciTypeCount };
enum { FFOsciBlockOn, FFOsciBlockType, FFOsciBlockNote, FFOsciAccGain, FFOsciAccCent, FFOsciAccPW, FFOsciAccGLFOToGain, FFOsciParamCount };