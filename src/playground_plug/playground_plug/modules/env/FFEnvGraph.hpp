#pragma once

class FBPlugGUI;
struct FBModuleGraphComponentData;

// TODO make it a struct
void
FFEnvRenderGraph(
  FBPlugGUI const* plugGUI,
  int moduleSlot,
  FBModuleGraphComponentData* data);