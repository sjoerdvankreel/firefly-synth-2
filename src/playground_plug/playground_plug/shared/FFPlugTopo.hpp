#pragma once

#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };
enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFShaperBlockOn, FFShaperBlockClip, FFShaperAccGain, FFShaperParamCount };
enum { FFOsciBlockOn, FFOsciBlockType, FFOsciAccGain, FFOsciAccPitch, FFOsciParamCount };