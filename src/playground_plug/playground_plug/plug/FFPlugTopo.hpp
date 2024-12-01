#pragma once

#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };

enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciAccGain, FFOsciAccPitch, FFOsciAccCount };
enum { FFOsciBlockOn, FFOsciBlockType, FFOsciBlockCount };

enum { FFShaperAccGain, FFShaperAccCount };
enum { FFShaperBlockOn, FFShaperBlockClip, FFShaperBlockCount };