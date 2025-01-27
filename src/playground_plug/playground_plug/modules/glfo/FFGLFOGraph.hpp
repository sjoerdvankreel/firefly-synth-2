#pragma once

struct FBRuntimeTopo;
struct FBScalarStateContainer;
struct FBModuleGraphComponentData;

// TODO make it a struct
void
FFGLFORenderGraph(
  FBRuntimeTopo const* topo,
  FBScalarStateContainer const* scalar,
  int moduleSlot,
  FBModuleGraphComponentData* data);