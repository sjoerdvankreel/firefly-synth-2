#pragma once

#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

// todo move module counts into here

enum { FFModuleGLFO, FFModuleOsci, FFModuleShaper, FFModuleMaster, FFModuleCount };
enum { FFGLFOBlockOn, FFGLFOAccRate, FFGLFOParamCount };
enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciBlockOn, FFOsciBlockType, FFOsciAccGain, FFOsciAccPitch, FFOsciAccGLFOToGain, FFOsciParamCount };
enum { FFShaperBlockOn, FFShaperBlockClip, FFShaperAccGain, FFShaperAccGLFOToGain, FFShaperParamCount };
enum { FFMasterAccGain, FFMasterParamCount };